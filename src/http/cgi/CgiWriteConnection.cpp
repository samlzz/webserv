/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWriteConnection.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:44:04 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/17 12:10:32 by sliziard         ###   ########.fr       */
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
	int stdinFd, const std::string &body, CgiProcess &ctx
)
	: AConnection(stdinFd, POLLOUT)
	, _body(body), _offset(0), _ctx(ctx)
{
	setNonBlocking();
}

// ============================================================================
// Methods
// ============================================================================

ConnEvent	CgiWriteConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		return (_ctx.onError(), ConnEvent::closeWith(_ctx.read()));

	if (revents & POLLOUT)
	{
		ssize_t n = write(_fd, _body.c_str() + _offset, _body.size() - _offset);
		if (n <= 0)
			return (_ctx.onError(), ConnEvent::closeWith(_ctx.read()));

		_offset += static_cast<size_t>(n);
		if (_offset == _body.size())
		{
			_ctx.onBodyEnd();
			return ConnEvent::close();
		}
	}
	return ConnEvent::none();
}
