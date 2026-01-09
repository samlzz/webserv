/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/08 18:09:25 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/poll.h>

#include "ClientConnection.hpp"
#include "config/Config.hpp"
#include "server/AConnection.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

ClientConnection::ClientConnection(int cliSockFd, const Config::Server &config)
	: AConnection(cliSockFd), _conf(config)
{}

// ============================================================================
// Methods
// ============================================================================

bool	ClientConnection::handleRead()
{
	return true;
}

bool	ClientConnection::handleWrite()
{
	return true;
}

bool	ClientConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		return false;
	bool	res;

	res = true;
	if (revents & POLLIN)
	{
		res &= handleRead();
	}
	if (revents & POLLOUT)
	{
		res &= handleWrite();
	}
	return res;
}