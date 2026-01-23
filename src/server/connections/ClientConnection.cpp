/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/23 21:29:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <ctime>
#include <ostream>
#include <sys/poll.h>
#include <sys/types.h>

#include "ClientConnection.hpp"
#include "config/Config.hpp"
#include "AConnection.hpp"
#include "ConnEvent.hpp"
#include "ft_log/LogOp.hpp"
#include "ft_log/level.hpp"
#include "log.h"

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
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_ERROR)
			<< "Fail to read " << CLIENT_READ_BUF_SIZE
			<< " bytes from fd " << _fd << std::endl;
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
	ConnEvent	ret = ConnEvent::none();
	if (_req.isDone())
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
			<< "Parsed request:\n"
			<< WS_LOG_SEP << '\n' << _req
			<< WS_LOG_SEP << std::endl;
		ret = _resp.build(_req, *this);
		if (ret.type == ConnEvent::CE_SPAWN)
			_cgiRead = ret.conn;

		_state = CS_WAIT_RESPONSE;
		_events = POLLOUT;
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
			<< "Built response for client " << _fd
			<< " , wait for POLLOUT..." << std::endl;
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
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_ERROR)
			<< "Fail to send " << buf.size() - _offset
			<< " bytes to fd " << _fd << std::endl;
		return ConnEvent::close();
	}

	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
		<< "Send " << n << " bytes to client on fd " << _fd << std::endl;
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_TRACE) << "Buffer sent:\n"
		<< WS_LOG_SEP << '\n' << buf.c_str() + _offset
		<< WS_LOG_SEP << std::endl;

	_tsLastActivity = std::time(0);
	_offset += static_cast<size_t>(n);

	// all curent buffer was sent
	if (_offset == buf.size())
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
			<< "Current buffer was totally sent" << std::endl;
		_resp.stream().pop();
		_offset = 0;

		if (!_resp.stream().hasChunk())
		{
			if (_resp.isDone())
			{
				ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
					<< "Response was sent to client " << _fd << std::endl;

				detachBuddy();
				if (_resp.shouldCloseConnection())
					return ConnEvent::close();

				_req.reset();
				_resp.reset();
				_state = CS_WAIT_REQUEST;
				_events = POLLIN;
				ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
					<< "Wait for POLLIN..." << std::endl;
			}
			else
			{
				_events = 0;
				ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
					<< "No buffer left in response stream, wait for it..." << std::endl;
			}
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
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_ERROR)
			<< "Poll error occurs on fd " << _fd << std::endl;
		return ConnEvent::close();
	}

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
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_WARN)
			<< "Client on fd " << _fd
			<< " has timeout when waiting for " << _state << std::endl;
		return ConnEvent::close();
	}

	_req.checkTimeout(now);
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

void	ClientConnection::notifyWritable(void)
{
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
		<< "Some buffer was added to response stream" << std::endl;
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
		<< "Wait for POLLOUT..." << std::endl;
	addEvent(POLLOUT);
}

IConnection	*ClientConnection::buddy(void)		{ return _cgiRead; }
void		ClientConnection::detachBuddy(void)	{ _cgiRead = 0; }
