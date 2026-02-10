/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 08:50:37 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/07 17:13:14 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>

#include "ServerConnection.hpp"
#include "AConnection.hpp"
#include "ClientConnection.hpp"
#include "ConnEvent.hpp"
#include "http/dispatch/HttpDispatcher.hpp"
#include "server/Exceptions.hpp"
#include "server/connections/IConnection.hpp"

// ============================================================================
// Construction 
// ============================================================================

ServerConnection::ServerConnection(const Config::Server &servConfig,
									const HttpDispatcher &dispatch)
	: AConnection(socket(
		AF_INET,
		SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
		0))
	, _ctx(dispatch, servConfig)
{
	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in	addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(servConfig.port);
	addr.sin_addr = servConfig.host;

	if (bind(_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
		throw SysError("bind");
	if (listen(_fd, LISTEN_QUEUE) < 0)
		throw SysError("listen");
}

// ============================================================================
// Methods
// ============================================================================

ConnEvent	ServerConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		throw LogicError("poll error on listen socket");
	if (revents & POLLHUP)
		throw LogicError("listen socket hung up");

	if (revents & POLLIN)
	{
		sockaddr_storage	addr;
		socklen_t			len = sizeof(addr);
		int					cliSockFd = accept(
									_fd,
									reinterpret_cast<struct sockaddr *>(&addr),
									&len);
		if (cliSockFd < 0)
		{
			if (isNonBlockingErrno())
				return ConnEvent::none();
			throw SysError("accept");
		}
		IConnection	*cliConn = new ClientConnection(cliSockFd, _ctx, addr);
		return ConnEvent::spawn(cliConn);
	}

	return ConnEvent::none();
}