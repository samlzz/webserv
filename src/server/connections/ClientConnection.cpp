/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/22 13:14:34 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <ctime>
#include <sys/poll.h>
#include <sys/types.h>

#include "ClientConnection.hpp"
#include "config/Config.hpp"
#include "AConnection.hpp"
#include "ConnEvent.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

ClientConnection::ClientConnection(int cliSockFd, const Config::Server &config)
	: AConnection(cliSockFd)
	, _req(), _resp(config)
	, _offset(0), _cgiRead(0)
	, _state(CS_WAIT_FIRST_BYTE)
	, _tsLastActivity(std::time(0))
{
	setNonBlocking();
}

// ============================================================================
// Private members methods
// ============================================================================

ConnEvent	ClientConnection::handleRead(void)
{
	char	buf[CLIENT_READ_BUF_SIZE];
	ssize_t	n = recv(_fd, buf, CLIENT_READ_BUF_SIZE, 0);

	if (n <= 0)
		return ConnEvent::close();

	_tsLastActivity = std::time(0);
	if (_state == CS_WAIT_FIRST_BYTE)
		_state = CS_WAIT_REQUEST;

	_req.feed(buf, static_cast<size_t>(n));

	ConnEvent	ret = ConnEvent::none();
	if (_req.isDone())
	{
		ret = _resp.build(_req, *this);
		if (ret.type == ConnEvent::CE_SPAWN)
			_cgiRead = ret.conn;

		_state = CS_WAIT_RESPONSE;
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

	_tsLastActivity = std::time(0);
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
				detachBuddy();
				if (_resp.shouldCloseConnection())
					return ConnEvent::close();

				_req.reset();
				_resp.reset();
				_state = CS_WAIT_REQUEST;
				_events = POLLIN;
			}
			else
				_events = 0;
		}
	}
	return ConnEvent::none();
}

// ============================================================================
// Public methods
// ============================================================================

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

time_t	ClientConnection::timeoutFromState(void)
{
	switch (_state) {
		case CS_WAIT_FIRST_BYTE:	return CLIENT_TIMEOUT_ACCEPT;
		case CS_WAIT_REQUEST:		return CLIENT_TIMEOUT_REQ;
		case CS_WAIT_RESPONSE:		return CLIENT_TIMEOUT_RESP;
		default:					return 0;
	}
}

ConnEvent	ClientConnection::checkTimeout(time_t now)
{
	time_t	timeout = timeoutFromState();

	if (difftime(now, _tsLastActivity) > timeout)
		return ConnEvent::close();

	// TODO call _req.checkTimeout(now)
	return ConnEvent::none();
}

void	ClientConnection::notifyWritable(void)
{
	addEvent(POLLOUT);
}

IConnection	*ClientConnection::buddy(void)		{ return _cgiRead; }
void		ClientConnection::detachBuddy(void)	{ _cgiRead = 0; }