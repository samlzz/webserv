/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 08:50:37 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/22 14:06:44 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>

#include "ServerConnection.hpp"
#include "AConnection.hpp"
#include "ClientConnection.hpp"
#include "ConnEvent.hpp"
#include "ft_log/LogOp.hpp"
#include "ft_log/level.hpp"
#include "log.h"
#include "server/Exceptions.hpp"

// ============================================================================
// Construction 
// ============================================================================

ServerConnection::ServerConnection(const Config::Server &servConfig)
	: AConnection(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))
	, _conf(servConfig)
{
	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in	addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_conf.port);
	addr.sin_addr = _conf.host;

	if (bind(_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
		throw SysError("bind");
	if (listen(_fd, LISTEN_QUEUE) < 0)
		throw SysError("listen");

	ft_log::log(WS_LOG_SERVER, ft_log::LOG_INFO)
		<< "Server listening on "
		<< inet_ntoa(_conf.host) << ":" << _conf.port
		<< " ..." << std::endl;
}

// ============================================================================
// Methods
// ============================================================================

ConnEvent	ServerConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		throw WsServerError("poll error on listen socket");

	if (revents & POLLIN)
	{
		int	cliSock = accept(_fd, 0, 0);
		if (cliSock < 0)
		{
			if (isNonBlockingErrno())
				return ConnEvent::none();
			throw SysError("accept");
		}
		return ConnEvent::spawn(new ClientConnection(cliSock, _conf));
	}

	return ConnEvent::none();
}