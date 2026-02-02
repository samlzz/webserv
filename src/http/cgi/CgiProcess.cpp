/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 16:12:31 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/02 09:45:08 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "CgiProcess.hpp"
#include "http/cgi/CgiReadConnection.hpp"
#include "http/cgi/CgiWriteConnection.hpp"
#include "http/cgi/IOutputSink.hpp"
#include "http/response/BuffStream.hpp"
#include "server/connections/IConnection.hpp"
#include "server/connections/IWritableNotifier.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

CgiProcess::CgiProcess(IOutputSink &sink)
	: _sink(sink), _notifier(0)
	, _pid(-1), _exitCode(0)
	, _terminated(false), _errOccur(false)
	, _read(0), _write(0), _refCount(0)
{}

CgiProcess::~CgiProcess()
{}

// ============================================================================
// Accessors
// ============================================================================

IConnection	*CgiProcess::read(void) const			{ return _read; }
IConnection	*CgiProcess::write(void) const			{ return _write; }

void		CgiProcess::retain(void)				{ _refCount++; }
void		CgiProcess::release(void)
{
	if (_refCount <= 0)
		return;
	--_refCount;
	if (_refCount == 0)
		delete this;
}

void		CgiProcess::forgetRead(void)			{ _read = 0; }
void		CgiProcess::forgetWrite(void)			{ _write = 0; }

time_t		CgiProcess::startTime(void) const		{ return _startTs; }

bool		CgiProcess::isTerminated(void) const	{ return _terminated; }
bool		CgiProcess::isError(void) const			{ return _errOccur; }

uint8_t		CgiProcess::exitCode(void) const		{ return _exitCode; }

void		CgiProcess::setDataNotify(IWritableNotifier *notifier)
{
	_notifier = notifier;
}

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


static inline void	freeCharArray(char **array)
{
	if (!array)
		return;
	for (size_t i = 0; array[i]; ++i)
		::free(array[i]);
	delete []array;
}

// never return
static t_never	_execChild(
	char **argv, char **envp,
	int stdoutFd, int stdinFd)
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

	execve(argv[0], argv, envp);
	freeCharArray(argv);
	freeCharArray(envp);
	_exit(1);
}

// Takes a vector of string and convert it to an allocated array of char ptr
static inline char**	toCharArray(const std::vector<std::string>& pVec)
{
	char** arr = new char*[pVec.size() + 1];

	for (size_t i = 0; i < pVec.size(); i++)
	{
		arr[i] = ::strdup(pVec[i].c_str());
		if (!arr[i])
		{
			for (size_t j = 0; j < i; ++j)
				::free(arr[j]);
			delete []arr;
			return 0;
		}
	}

	arr[pVec.size()] = NULL;
	return arr;
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
IConnection	*CgiProcess::start(const std::vector<std::string> &argv,
								const std::vector<std::string> &envp,
								const t_bytes &body)
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
	if (pid == 0) /* child process */
	{
		_closeFds(outFds[0], inFds[1]);
		_execChild(toCharArray(argv), toCharArray(envp), outFds[1], inFds[0]);
	}
	else /* parent process */
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

		_startTs = std::time(0);
		_pid = pid;
		_read = readConn;
		_write = writeConn;
	}

	return _read;
}

void	CgiProcess::kill(void)
{
	if (_terminated)
		return;
	_terminated = true;
	cleanup(true);
}

// ---- Private members ----

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
		::kill(_pid, SIGKILL);

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

void	CgiProcess::onError(void)
{
	if (_terminated)
		return;
	_terminated = true;
	_errOccur = true;
	cleanup(true);
	if (_notifier)
		_notifier->notifyEnd();
}

void	CgiProcess::onTimeout(void)
{
	onError();
}

void	CgiProcess::onEof(void)
{
	if (_terminated)
		return;
	_terminated = true;
	forgetRead();
	_sink.finalize();
	if (_notifier)
		_notifier->notifyEnd();
	cleanup(false);
}

void	CgiProcess::onRead(const char *buffer, size_t bufSize)
{
	if (_terminated)
		return ;
	_sink.append(buffer, bufSize);
	if (_notifier)
		_notifier->notifyWritable();
}