/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/21 19:43:54 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
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
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_ERROR)
			<< "Fail to read " << CLIENT_READ_BUF_SIZE
			<< " bytes from fd " << _fd << std::endl;
		return ConnEvent::close();
	}

	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
		<< "Received " << n << " bytes from client on fd " << _fd << std::endl;
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
		<< WS_LOG_SEP << "\n" << std::string(buf, n)
		<< WS_LOG_SEP << std::endl;

	_req.feed(buf, static_cast<size_t>(n));

	ConnEvent	ret = ConnEvent::none();
	if (_req.isDone())
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
			<< WS_LOG_SEP << "\n" << "Parsed request: \n" << _req
			<< WS_LOG_SEP << std::endl;
		ret = _resp.build(_req, *this);
		_events = POLLOUT;
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
			<< "Build response for client " << _fd
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

	_offset += static_cast<size_t>(n);

	// all curent buffer was sent
	if (_offset == buf.size())
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
			<< "Current buffer was totally sent" << std::endl;
		ft_log::log(WS_LOG_SERVER_CLI)
			<< WS_LOG_SEP << "\n" << buf << WS_LOG_SEP << std::endl;
		_resp.stream().pop();
		_offset = 0;

		if (!_resp.stream().hasChunk())
		{
			if (_resp.isDone())
			{
				ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
					<< "Sent response to client " << _fd << std::endl;

				if (_resp.shouldCloseConnection())
					return ConnEvent::close();

				_req.reset();
				_resp.reset();
				_events = POLLIN;
				ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
					<< "Wait for POLLIN..." << std::endl;
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
	{
		ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_ERROR)
			<< "Poll error occurs on fd " << _fd;
		return ConnEvent::close();
	}

	if (revents & POLLIN)
		return handleRead();

	if (revents & POLLOUT)
		return handleWrite();

	return ConnEvent::none();
}

void	ClientConnection::notifyWritable(void)
{
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_DEBUG)
		<< "Some buffer was added to response stream" << std::endl;
	ft_log::log(WS_LOG_SERVER_CLI, ft_log::LOG_INFO)
		<< "Wait for POLLOUT..." << std::endl;
	addEvent(POLLOUT);
}