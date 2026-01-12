/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/09 21:08:43 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/poll.h>

#include "ClientConnection.hpp"
#include "config/Config.hpp"
#include "AConnection.hpp"
#include "ConnEvent.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

ClientConnection::ClientConnection(int cliSockFd, const Config::Server &config)
	: AConnection(cliSockFd), _conf(config)
{
	setNonBlocking();
}

// ============================================================================
// Methods
// ============================================================================

bool	ClientConnection::processIO(short revents)
{
/* TODO: Expected logic
	if ((revents & POLLIN) && !_request.done())
		_request.consume(_fd);

	if ((revents & POLLOUT) && _request.done())
		_response.produce(_fd);

	if (_response.done())
	{
		if (_response.shouldClose())
			return false;

		_request.reset();
		_response.reset();
	}
*/
	if (revents & POLLIN)
	{
		char	buf[1024];
		ssize_t	n = recv(_fd, buf, sizeof(buf) - 1, 0);

		if (n > 0)
		{
			buf[n] = 0;
			std::cout << "Received from fd " << _fd << ":\n" << buf << std::endl;
			_events = POLLOUT;
		}
		else if (n <= 0)
			return false;
	}
	if (revents & POLLOUT)
	{
		std::string	resp = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
		ssize_t		sent = send(_fd, resp.c_str(), resp.length(), 0);

		if (sent < 0)
			std::cerr << "failed to send response to fd " << _fd << std::endl;
		else
			std::cout << "Response sent to fd " << _fd << std::endl;
		_events = POLLIN;
	}
	return true;
}

ConnEvent	ClientConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		return ConnEvent::close();

	if (!processIO(revents))
		return ConnEvent::close();

	return ConnEvent::none();
}