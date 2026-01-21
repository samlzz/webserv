/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiReadConnection.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/20 19:01:59 by sliziard         ###   ########.fr       */
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
	: AConnection(stdoutFd, POLLIN), _ctx(ctx)
{
	setNonBlocking();
}

// ============================================================================
// Methods
// ============================================================================

IConnection	*CgiReadConnection::buddy(void)
{
	return _ctx.write();
}

void	CgiReadConnection::detachBuddy(void)
{
	_ctx.forgetWrite();
}

ConnEvent	CgiReadConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		return (_ctx.onError(), ConnEvent::close());

	if (revents & POLLIN)
	{
		char	buf[CGI_READ_BUF_SIZE];
		ssize_t	n = read(_fd, buf, CGI_READ_BUF_SIZE);

		if (n < 0)
			return (ConnEvent::none());
		if (n == 0)
			return (_ctx.onEof(), ConnEvent::close());

		_ctx.onRead(buf, static_cast<size_t>(n));
	}
	return ConnEvent::none();
}