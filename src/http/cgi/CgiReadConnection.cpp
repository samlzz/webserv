/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiReadConnection.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/19 19:11:11 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
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
	: AConnection(stdoutFd, POLLIN)
	, _ctx(ctx), _writeSpawned(false)
{
	setNonBlocking();
}

// ============================================================================
// Methods
// ============================================================================

static inline ConnEvent	_getCloseEvent(IConnection *bindedConn)
{
	if (bindedConn)
		return ConnEvent::closeWith(bindedConn);
	return ConnEvent::close();
}

ConnEvent	CgiReadConnection::handleEvents(short revents)
{
	if (!_writeSpawned)
	{
		_writeSpawned = true;
		return ConnEvent::spawn(_ctx.write());
	}

	if (isErrEvent(revents))
		return (_ctx.onError(), _getCloseEvent(_ctx.write()));

	if (revents & POLLIN)
	{
		char	buf[CGI_READ_BUF_SIZE];
		ssize_t	n = read(_fd, buf, CGI_READ_BUF_SIZE);

		if (n < 0)
			return (_ctx.onError(), _getCloseEvent(_ctx.write()));
		if (n == 0)
			return (_ctx.onEof(), _getCloseEvent(_ctx.write()));

		_ctx.onRead(buf, static_cast<size_t>(n));
	}
	return ConnEvent::none();
}

