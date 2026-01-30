/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/30 18:04:51 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <ctime>
#include <sys/poll.h>
#include <sys/types.h>

#include "ClientConnection.hpp"
#include "AConnection.hpp"
#include "ConnEvent.hpp"
#include "http/cgi/INeedsNotifier.hpp"
#include "http/response/HttpResponse.hpp"
#include "http/response/BuffStream.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/response/interfaces/IFifoStream.hpp"
#include "http/routing/Router.hpp"
#include "server/ServerCtx.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

ClientConnection::ClientConnection(int cliSockFd, const ServerCtx &serverCtx)
	: AConnection(cliSockFd), _serv(serverCtx)
	, _req(), _resp(0)
	, _offset(0), _cgiRead(0)
	, _shouldRefresh(false)
	, _state(CS_WAIT_FIRST_BYTE)
	, _tsLastActivity(std::time(0))
{
	setFdFlags();
}

// ============================================================================
// Private members methods
// ============================================================================

ConnEvent	ClientConnection::buildResponse(void)
{
	routing::Context	route = routing::resolve(_req, _serv);
	ResponsePlan		plan = _serv.dispatcher.dispatch(_req, route);
	INeedsNotifier		*needs = dynamic_cast<INeedsNotifier *>(plan.body);

	if (needs)
		needs->setNotifier(*this);
	if (plan.event.conn && plan.event.type == ConnEvent::CE_SPAWN)
		_cgiRead = plan.event.conn;

	_resp = new HttpResponse(plan, _req, route);
	return plan.event;
}

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
	if (_req.isDone())
	{
		_state = CS_WAIT_RESPONSE;
		_events = POLLOUT;
		return buildResponse();
	}
	return ConnEvent::none();
}

ConnEvent	ClientConnection::handleWrite(void)
{
	if (!_resp->fillStream())
		return ConnEvent::close();

	IFifoStreamView<t_bytes>	&stream = _resp->stream();
	if (!stream.hasBuffer())
		return (_events = 0, ConnEvent::none());
	const t_bytes				&buf = stream.front();

	if (buf.empty())
		return (stream.pop(), ConnEvent::none());
	// ? _offset must always be strictly less than buf.size()
	ssize_t n = send(_fd,
					buf.data() + _offset,
					buf.size() - _offset,
					0);
	if (n <= 0) // ? so n should never be 0
		return ConnEvent::close();

	_tsLastActivity = std::time(0);
	_offset += static_cast<size_t>(n);

	// ? all curent buffer was sent
	if (_offset == buf.size())
	{
		stream.pop();
		_offset = 0;

		if (!stream.hasBuffer())
		{
			if (_resp->isDone())
			{
				detachBuddy();
				if (_resp->shouldCloseConnection())
					return ConnEvent::close();

				_req.reset();
				delete _resp;
				_resp = 0;
				_state = CS_WAIT_REQUEST;
				_events = POLLIN;
			}
			else
				_events = 0; // ? No buffer to send, wait for notifyWritable
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

	return exitEvent(revents);
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

	_req.checkTimeout(now);
	if (_shouldRefresh)
	{
		_shouldRefresh = false;
		return ConnEvent::refresh();
	}
	return ConnEvent::none();
}

void	ClientConnection::notifyWritable(void)
{
	addEvent(POLLOUT);
	_shouldRefresh = true;
}

IConnection	*ClientConnection::buddy(void)		{ return _cgiRead; }
void		ClientConnection::detachBuddy(void)	{ _cgiRead = 0; }