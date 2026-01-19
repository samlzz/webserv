/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:32:44 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/19 19:05:40 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_PROCESS_HPP__
# define __CGI_PROCESS_HPP__

# include <string>
# include <sys/types.h>
# include <stdint.h>

# include "http/_response/IChunkEncoder.hpp"
# include "server/connections/IConnection.hpp"
# include "server/connections/IWritableNotifier.hpp"

class CgiReadConnection;
class CgiWriteConnection;

class CgiProcess {

private:
	IChunkEncoder		&_encoder;
	IWritableNotifier	&_notifier;
	pid_t				_pid;
	uint8_t				_exitCode;
	bool				_terminated;
	CgiReadConnection	*_read;
	CgiWriteConnection	*_write;

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

	bool				isDone(void) const;
	uint8_t				exitCode(void) const;

	// ============================================================================
	// Methods
	// ============================================================================

	CgiReadConnection	*start(const char* scriptPath,
								char* const argv[],
								char* const envp[],
								const std::string &body = "");

	void				onEof(void);
	void				onBodyEnd(void);

	void				onError(void);
	void				onRead(const char *buffer, size_t bufSize);

private:

	void				cleanup(bool killChild);

	// forbidden
	CgiProcess();
	CgiProcess(const CgiProcess &other);
	CgiProcess& operator=(const CgiProcess &other);
};

#endif /* __CGI_PROCESS_HPP__ */