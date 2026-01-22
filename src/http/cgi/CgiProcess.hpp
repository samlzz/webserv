/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:32:44 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/22 15:34:05 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_PROCESS_HPP__
# define __CGI_PROCESS_HPP__

# include <cstddef>
# include <ctime>
# include <string>
# include <sys/types.h>
# include <stdint.h>

# include "http/response/IChunkEncoder.hpp"
# include "server/connections/IConnection.hpp"
# include "server/connections/IWritableNotifier.hpp"

# ifndef CGI_MAX_EXEC_TIME
#  define CGI_MAX_EXEC_TIME 30000 // 30 s
# endif

// ============================================================================
// Type Utils
// ============================================================================
typedef void	t_never;

// ============================================================================
// Cgi context class
// It share data between caller and read/write connections
// ============================================================================
class CgiProcess {

private:
	// ============================================================================
	// Attributes
	// ============================================================================
	IChunkEncoder		&_encoder;
	IWritableNotifier	&_notifier;
	pid_t				_pid;
	time_t				_startTs;
	uint8_t				_exitCode;
	bool				_terminated;
	IConnection			*_read;
	IConnection			*_write;

public:

	// ============================================================================
	// Construction
	// ============================================================================
	CgiProcess(IChunkEncoder &encoder, IWritableNotifier &notifier);
	~CgiProcess();

	// ============================================================================
	// Accessors
	// ============================================================================

	IConnection			*read(void) const;
	IConnection			*write(void) const;
	void				forgetRead(void);
	void				forgetWrite(void);

	time_t				startTime(void) const;

	bool				isDone(void) const;
	uint8_t				exitCode(void) const;

	// ============================================================================
	// Methods
	// ============================================================================

	IConnection			*start(const char* scriptPath,
								char* const argv[],
								char* const envp[],
								const std::string &body = "");

	void				onError(void);

	// ---- CgiReadConnection events ----
	void				onRead(const char *buffer, size_t bufSize);
	void				onEof(void);
	void				onTimeout(void);

	// ---- CgiReadConnection events ----
	void				onBodyEnd(size_t writtenBytes);

private:

	void				cleanup(bool killChild);

	// forbidden
	CgiProcess();
	CgiProcess(const CgiProcess &other);
	CgiProcess& operator=(const CgiProcess &other);
};

#endif /* __CGI_PROCESS_HPP__ */