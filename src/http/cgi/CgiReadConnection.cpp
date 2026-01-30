/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiReadConnection.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/30 17:29:50 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <ctime>
#include <sys/poll.h>
#include <unistd.h>

#include "CgiReadConnection.hpp"
#include "http/cgi/CgiProcess.hpp"
#include "server/connections/AConnection.hpp"
#include "server/connections/ConnEvent.hpp"
#include "server/connections/IConnection.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

CgiReadConnection::CgiReadConnection(int stdoutFd, CgiProcess &ctx)
	: AConnection(stdoutFd, POLLIN), _ctx(ctx)
{
	_ctx.retain();
	setFdFlags();
}

CgiReadConnection::~CgiReadConnection()
{
	_ctx.release();
}

// ============================================================================
// Methods
// ============================================================================

ConnEvent	CgiReadConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		return (_ctx.onError(), ConnEvent::close());

	if (revents & POLLIN)
	{
		char	buf[CGI_READ_BUF_SIZE];
		ssize_t	n = read(_fd, buf, CGI_READ_BUF_SIZE);

		if (n < 0)
			return (_ctx.onError(), ConnEvent::close());
		if (n == 0)
			return (_ctx.onEof(), ConnEvent::close());

		_ctx.onRead(buf, static_cast<size_t>(n));
	}
	return exitEvent(revents);
}

ConnEvent	CgiReadConnection::checkTimeout(time_t now)
{
	if (difftime(now, _ctx.startTime()) > CGI_MAX_EXEC_TIME)
	{
		_ctx.onTimeout();
		return ConnEvent::close();
	}
	return ConnEvent::none();
}

IConnection	*CgiReadConnection::buddy(void)			{ return _ctx.write(); }
void		CgiReadConnection::detachBuddy(void)	{ _ctx.forgetWrite(); }