/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 08:50:37 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/08 17:56:35 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>

#include "ServerConnection.hpp"
#include "AConnection.hpp"
#include "ClientConnection.hpp"
#include "IConnection.hpp"
#include "server/Exceptions.hpp"

// ============================================================================
// Construction 
// ============================================================================

ServerConnection::ServerConnection(const Config::Server &servConfig,
										std::vector<IConnection *> &connexions)
	: AConnection(socket(AF_INET, SOCK_STREAM, 0))
	, _conf(servConfig)
	, _pfds(connexions)
{
	sockaddr_in	addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_conf.port);
	addr.sin_addr = _conf.host;

	if (bind(_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
		throw SysError("bind");
	if (listen(_fd, LISTEN_QUEUE) < 0)
		throw SysError("listen");
	connexions.push_back(this);
}

// ============================================================================
// Methods
// ============================================================================

bool	ServerConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		return false;
	if (revents & POLLIN)
	{
		int	cliSock = accept(_fd, 0, 0);
		if (cliSock < 0)
		{
			if (isNonBlockingErrno())
				return true;
			throw SysError("accept");
		}
		ClientConnection	*client = new ClientConnection(cliSock, _conf);
		_pfds.push_back(client);
	}
	return true;
}