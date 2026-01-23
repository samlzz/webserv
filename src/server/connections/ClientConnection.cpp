/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/23 02:44:55 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sys/poll.h>
#include <sys/types.h>

#include "ClientConnection.hpp"
#include "config/Config.hpp"
#include "AConnection.hpp"
#include "ConnEvent.hpp"

#include <iostream>

// ============================================================================
// Construction / Destruction
// ============================================================================

ClientConnection::ClientConnection(int cliSockFd, const Config::Server &config)
	: AConnection(cliSockFd), _req(), _resp(config)
{
	setNonBlocking();
}

// ============================================================================
// Methods
// ============================================================================

ConnEvent	ClientConnection::handleRead(void)
{
	char	buf[CLIENT_READ_BUF_SIZE];
	ssize_t	n = recv(_fd, buf, CLIENT_READ_BUF_SIZE, 0);

	if (n <= 0)
		return ConnEvent::close();

	_req.feed(buf, static_cast<size_t>(n));
	ConnEvent	ret = ConnEvent::none();
	if (_req.isDone())
	{
		ret = _resp.build(_req, *this);
		_events = POLLOUT;
	}
	return ret;
}

ConnEvent	ClientConnection::handleWrite(void)
{
	_resp.fillStream();
	const std::string	&buf = _resp.stream().front();

	ssize_t n = send(_fd,
					buf.c_str() + _offset,
					buf.size() - _offset,
					0);
	if (n <= 0)
		return ConnEvent::close();

	_offset += static_cast<size_t>(n);

	// all curent buffer was sent
	if (_offset == buf.size())
	{
		_resp.stream().pop();
		_offset = 0;

		if (!_resp.stream().hasChunk())
		{
			if (_resp.isDone())
			{
				if (_resp.shouldCloseConnection())
					return ConnEvent::close();

				_req.reset();
				_resp.reset();
				_events = POLLIN;
			}
			else
				_events = 0;
		}
	}
	return ConnEvent::none();
}



ConnEvent	ClientConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		return ConnEvent::close();

	if (revents & POLLIN)
		return handleRead();

	if (revents & POLLOUT)
		return handleWrite();

	return ConnEvent::none();
}

void	ClientConnection::notifyWritable(void)
{
	addEvent(POLLOUT);
}