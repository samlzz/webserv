/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWriteConnection.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:44:04 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 16:49:01 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sys/types.h>
#include <unistd.h>

#include "CgiWriteConnection.hpp"
#include "http/cgi/CgiProcess.hpp"
#include "server/connections/AConnection.hpp"
#include "server/connections/ConnEvent.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

CgiWriteConnection::CgiWriteConnection(
	int stdinFd, const t_bytes &body, CgiProcess &ctx
)
	: AConnection(stdinFd, POLLOUT)
	, _body(body), _offset(0), _ctx(ctx), _spawned(false)
{
	_ctx.retain();
	setFdFlags();
}

CgiWriteConnection::~CgiWriteConnection()
{
	_ctx.release();
}

// ============================================================================
// Methods
// ============================================================================

IConnection	*CgiWriteConnection::buddy(void)
{
	if (!_spawned)
		return 0;
	return _ctx.read();
}

void	CgiWriteConnection::detachBuddy(void)
{
	if (_spawned)
		_ctx.forgetRead();
}

ConnEvent	CgiWriteConnection::handleEvents(short revents)
{
	_spawned = true;
	if (isErrEvent(revents))
		return (_ctx.onError(), ConnEvent::close());

	if (revents & POLLOUT)
	{
		ssize_t n = write(_fd, _body.data() + _offset, _body.size() - _offset);
		if (n <= 0)
			return (ConnEvent::none());

		_offset += static_cast<size_t>(n);
		if (_offset == _body.size())
		{
			_ctx.onBodyEnd();
			_spawned = false;
			return ConnEvent::close();
		}
	}
	return ConnEvent::none();
}
