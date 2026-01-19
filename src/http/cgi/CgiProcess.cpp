/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 16:12:31 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/19 19:05:37 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "CgiProcess.hpp"
#include "http/_response/IChunkEncoder.hpp"
#include "http/cgi/CgiReadConnection.hpp"
#include "http/cgi/CgiWriteConnection.hpp"
#include "server/connections/IConnection.hpp"
#include "server/connections/IWritableNotifier.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

CgiProcess::CgiProcess(IChunkEncoder &encoder, IWritableNotifier &notifier)
	: _encoder(encoder), _notifier(notifier)
	, _pid(-1), _exitCode(0), _terminated(false)
	, _read(NULL), _write(NULL)
{}

CgiProcess::~CgiProcess()
{}

// ============================================================================
// Accessors
// ============================================================================

IConnection	*CgiProcess::read(void) const		{ return _read; }
IConnection	*CgiProcess::write(void) const		{ return _write; }

bool		CgiProcess::isDone(void) const		{ return _terminated; }

uint8_t		CgiProcess::exitCode(void) const	{ return _exitCode; }

// ============================================================================
// Helpers for start()
// ============================================================================

static inline void	_closeFds(int fd1, int fd2)
{
	if (fd1 >= 0)
		close(fd1);
	if (fd2 >= 0)
		close(fd2);
}

static inline void	_closeFds(int fds[2])
{
	_closeFds(fds[0], fds[1]);
}

// never return
static void	_execChild(
	const char* scriptPath,
	char* const argv[],
	char* const envp[],
	int stdoutFd,
	int stdinFd)
{
	if (dup2(stdoutFd, STDOUT_FILENO) < 0)
	{
		_closeFds(stdoutFd, stdinFd);
		_exit(1);
	}
	close(stdoutFd);

	if (stdinFd > -1)
	{
		int dupFailed = dup2(stdinFd, STDIN_FILENO) < 0;
		close(stdinFd);
		if (dupFailed)
			_exit(1);
	}

	execve(scriptPath, argv, envp);
	_exit(1);
}

// ============================================================================
// Methods
// ============================================================================

/**
 * fds[0] == read
 * fds[1] == write

 * child:
 *	- read from stdin (inFds[0])
 *	- write to stdout (outFds[1])

 * parent:
 *	- write to stdin (inFds[1])
 *	- read from stdout (outFds[0])
 */

// Returned connection must be spawned for the reactor
// CgiProcess doesn't owns it
CgiReadConnection	*CgiProcess::start(const char* scriptPath,
										char* const argv[],
										char* const envp[],
										const std::string &body)
{
	int	outFds[2];
	int	inFds[2] = {-1, -1};

	assert(_pid == -1 && "start cannot be called twice");
	if (pipe(outFds) < 0)
		return 0;
	if (!body.empty() && pipe(inFds) < 0)
	{
		int savedErrno = errno;
		_closeFds(outFds);
		errno = savedErrno;
		return 0;
	}

	pid_t	pid = fork();
	if (pid < 0)
	{
		int savedErrno = errno;
		_closeFds(outFds);
		_closeFds(inFds);
		errno = savedErrno;
		return 0;
	}
	if (pid == 0)
	{
		_closeFds(outFds[0], inFds[1]);
		_execChild(scriptPath, argv, envp, outFds[1], inFds[0]);
	}
	else
	{
		_closeFds(outFds[1], inFds[0]);

		CgiReadConnection	*readConn = NULL;
		CgiWriteConnection	*writeConn = NULL;
		try {
			readConn = new CgiReadConnection(outFds[0], *this);
			outFds[0] = -1;
			if (!body.empty())
			{
				writeConn = new CgiWriteConnection(inFds[1], body, *this);
				inFds[1] = -1;
			}
		}
		catch (...) {
			delete readConn;
			delete writeConn;
			_closeFds(outFds[0], inFds[1]);
			onError();
			return 0;
		}

		_pid = pid;
		_read = readConn;
		_write = writeConn;
	}

	return _read;
}

/**
 * `killChild` child must be true in case of error
 *
 * waitpid try to retrieve child status code
 * If child not terminated yet, ignore status code
 * and let the Reactor reap it
 */
void	CgiProcess::cleanup(bool killChild)
{
	if (_pid < 0)
		return;

	if (killChild)
		kill(_pid, SIGKILL);

	int		status = 0;
	pid_t	r = waitpid(_pid, &status, WNOHANG);

	if (r == _pid)
	{
		_pid = -1;
		if (WIFEXITED(status))
			_exitCode = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			_exitCode = 128 + WTERMSIG(status);
		else
			_exitCode = 255;
	}
}

// ============================================================================
// Events driven methods
// ============================================================================

void	CgiProcess::onBodyEnd(void)
{
	_write = NULL;
}

void	CgiProcess::onEof(void)
{
	if (_terminated)
		return;
	_terminated = true;
	_read = NULL;
	_encoder.finalize();
	cleanup(false);
}

void	CgiProcess::onError(void)
{
	if (_terminated)
		return;
	_terminated = true;
	cleanup(true);
}

void	CgiProcess::onRead(const char *buffer, size_t bufSize)
{
	_encoder.encode(buffer, bufSize);
	_notifier.notifyWritable();
}
