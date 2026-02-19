/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:32:44 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/19 13:16:43 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_PROCESS_HPP__
# define __CGI_PROCESS_HPP__

# include <cstddef>
# include <ctime>
# include <string>
# include <sys/types.h>
# include <stdint.h>
# include <vector>

# include "http/cgi/IOutputSink.hpp"
# include "http/response/BuffStream.hpp"
# include "server/connections/IConnection.hpp"
# include "server/connections/IWritableNotifier.hpp"

# ifndef CGI_MAX_EXEC_TIME
#  define CGI_MAX_EXEC_TIME 30 // ? in seconds
# endif

// ============================================================================
// Type Utils
// ============================================================================
typedef void	t_never;

class CgiChildExit : public std::exception {
private:
	uint8_t	_exitCode;

public:
	CgiChildExit(uint8_t exitCode) : _exitCode(exitCode) {}
	virtual ~CgiChildExit() throw() {}

	virtual const char* what() const throw()
	{
		return "CGI child process exit";
	}

	uint8_t exitCode() const throw() { return _exitCode; }
};

// ============================================================================
// Cgi context class
// It share data between caller and read/write connections
// ============================================================================
class CgiProcess {

private:
	// ============================================================================
	// Attributes
	// ============================================================================
	IOutputSink			&_sink;
	IWritableNotifier	*_notifier;
	
	pid_t				_pid;
	uint8_t				_exitCode;
	
	time_t				_startTs;

	bool				_terminated;
	bool				_errOccur;
	bool				_hasTimeout;

	IConnection			*_read;
	IConnection			*_write;
	int32_t				_refCount;

public:

	// ============================================================================
	// Construction
	// ============================================================================
	CgiProcess(IOutputSink &sink);
	~CgiProcess();

	// ============================================================================
	// Accessors
	// ============================================================================

	IConnection			*read(void) const;
	IConnection			*write(void) const;
	void				forgetRead(void);
	void				forgetWrite(void);

	void				retain(void);
	void				release(void);

	time_t				startTime(void) const;

	bool				isTerminated(void) const;
	bool				isError(void) const;
	bool				isTimeout(void) const;

	uint8_t				exitCode(void) const;

	void				setDataNotify(IWritableNotifier *notifier);

	// ============================================================================
	// Methods
	// ============================================================================

	IConnection			*start(const std::vector<std::string> &argv,
								const std::vector<std::string> &envp,
								const t_bytes &body);
	
	void				kill(void);

	// ============================================================================
	// Events driven methods
	// ============================================================================

	void				onError(void);

	// ---- CgiReadConnection events ----
	void				onRead(const char *buffer, size_t bufSize);
	void				onEof(void);
	void				onTimeout(void);

private:

	void				cleanup(bool killChild);

	// forbidden
	CgiProcess();
	CgiProcess(const CgiProcess &other);
	CgiProcess& operator=(const CgiProcess &other);
};

#endif /* __CGI_PROCESS_HPP__ */