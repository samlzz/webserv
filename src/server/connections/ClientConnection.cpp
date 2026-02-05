/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/05 18:26:03 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <ctime>
#include <ostream>
#include <sys/poll.h>
#include <sys/types.h>

#include "ClientConnection.hpp"
#include "AConnection.hpp"
#include "ConnEvent.hpp"
#include "ft_log/LogOp.hpp"
#include "ft_log/level.hpp"
#include "log.h"
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
	, _req(_serv.config.maxBodySize), _resp(0)
	, _offset(0), _cgiRead(0)
	, _shouldRefresh(false)
	, _state(CS_WAIT_FIRST_BYTE)
	, _tsLastActivity(std::time(0))
{
	setFdFlags();
}

ClientConnection::~ClientConnection(void)
{
	delete _resp;
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
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
		<< "Built response for client " << _fd
		<< " , wait for POLLOUT..." << std::endl;

	return plan.event;
}

/**
 * This function was call only when _req.isDnone is false.
 * Because when it's done we wait for POLLOUT only
*/
ConnEvent	ClientConnection::handleRead(void)
{
	char	buf[CLIENT_READ_BUF_SIZE];
	ssize_t	n = recv(_fd, buf, CLIENT_READ_BUF_SIZE, 0);

	// ? If EOF was received: client sent an incomplete request
	if (n <= 0) // ? error or EOF
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_ERROR)
			<< "Fail to read " << CLIENT_READ_BUF_SIZE
			<< " bytes from fd " << _fd << (n == 0 ? " bc EOF" : "") << std::endl;
		return ConnEvent::close();
	}

	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
		<< "Received " << n << " bytes from client on fd " << _fd << std::endl;
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_TRACE)
		<< "Received buffer:\n"
		<< WS_LOG_SEP << '\n' << std::string(buf, n)
		<< WS_LOG_SEP << std::endl;

	_tsLastActivity = std::time(0);
	if (_state == CS_WAIT_FIRST_BYTE)
		_state = CS_WAIT_REQUEST;

	_req.feed(buf, static_cast<size_t>(n));
	if (_req.isDone())
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
			<< "Parsed request:\n"
			<< WS_LOG_SEP << '\n' << _req
			<< WS_LOG_SEP << std::endl;

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
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_ERROR)
			<< "Fail to send " << buf.size() - _offset
			<< " bytes to fd " << _fd << std::endl;
		return ConnEvent::close();
	}

	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
		<< "Send " << n << " bytes to client on fd " << _fd << std::endl;
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_TRACE) << "Buffer sent:\n"
		<< WS_LOG_SEP << '\n' << std::string(buf.data() + _offset, buf.size() - _offset)
		<< WS_LOG_SEP << std::endl;

	_tsLastActivity = std::time(0);
	_offset += static_cast<size_t>(n);

	if (_offset == buf.size()) // ? all curent buffer was sent
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
			<< "Current buffer was totally sent" << std::endl;
		stream.pop();
		_offset = 0;

		if (!stream.hasBuffer())
		{
			if (_resp->isDone())
			{
				ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
					<< "Response was sent to client " << _fd << std::endl;

				detachBuddy();
				if (_resp->shouldCloseConnection())
					return ConnEvent::close();

				_req.reset();
				delete _resp;
				_resp = 0;
				_state = CS_WAIT_REQUEST;
				_events = POLLIN;
				ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
					<< "Wait for POLLIN..." << std::endl;
			}
			else
			{
				_events = 0; // ? No buffer to send, wait for notifyWritable
				ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
					<< "No buffer left in response stream, wait for it..." << std::endl;
			}
		}
	}
	return ConnEvent::none();
}

// ============================================================================
// IConnection methods
// ============================================================================

ConnEvent	ClientConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_ERROR)
			<< "Poll error occurs on fd " << _fd << std::endl;
		return ConnEvent::close();
	}

	if (revents & POLLIN)
		return handleRead();

	if (revents & POLLOUT)
		return handleWrite();

	if (revents & POLLHUP)
		return ConnEvent::close();

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
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_WARN)
			<< "Client on fd " << _fd
			<< " timeout when waiting for " << _state << std::endl;
		return ConnEvent::close();
	}

	_req.checkTimeout(now);
	if (_shouldRefresh)
	{
		_shouldRefresh = false;
		return ConnEvent::refresh();
	}
	return ConnEvent::none();
}

std::ostream	&operator<<(std::ostream &os, const ClientConnection::e_client_state &state)
{
	switch (state) {
		case ClientConnection::CS_WAIT_FIRST_BYTE:	os << "client first bytes sent"; break;
		case ClientConnection::CS_WAIT_REQUEST:		os << "client request"; break;
		case ClientConnection::CS_WAIT_RESPONSE:	os << "client receive"; break;
	}
	return os;
}
IConnection	*ClientConnection::buddy(void)		{ return _cgiRead; }
void		ClientConnection::detachBuddy(void)	{ _cgiRead = 0; }

// ============================================================================
// IWritableNotifier methods
// ============================================================================

void	ClientConnection::notifyWritable(void)
{
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
		<< "Some buffer was added to response stream" << std::endl;
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
		<< "Wait for POLLOUT..." << std::endl;
	addEvent(POLLOUT);
	_shouldRefresh = true;
}

// ? detach buddy and schedule a write attempt so HttpResponse can finalize (error/EOF)
void	ClientConnection::notifyEnd(void)
{
	detachBuddy();
	notifyWritable();
}
