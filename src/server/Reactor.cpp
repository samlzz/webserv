/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:21:31 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/09 14:59:11 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Reactor.hpp"
#include "server/Exceptions.hpp"
#include <cstddef>

// ============================================================================
// Construction / Destruction
// ============================================================================

Reactor::Reactor(): _running(false)
{}

Reactor::~Reactor()
{
	for (size_t i = 0; i < _connections.size(); ++i)
		delete _connections[i];
}

// ============================================================================
// Methods
// ============================================================================

void	Reactor::removeConnection(size_t idx)
{
	delete _connections[idx];

	_connections[idx] = _connections.back();
	_pfds[idx] = _pfds.back();

	_connections.pop_back();
	_pfds.pop_back();
}


void	Reactor::addConnection(IConnection* conn)
{
	_connections.push_back(conn);
	_pfds.push_back(conn->pollFd());
}

void	Reactor::stop(void)
{
	_running = false;
}

// Main poll listen loop
void	Reactor::run(void)
{
	_running = true;
	while (_running)
	{
		int ret = poll(_pfds.data(), _pfds.size(), POLL_TIMEOUT);
		if (ret < 0)
			throw SysError("poll");

		for (size_t i = 0; i < _pfds.size(); ++i)
		{
			if (_pfds[i].revents == 0)
				continue;

			IConnection	*conn = _connections[i];
			ConnEvent	ev = conn->handleEvents(_pfds[i].revents);

			if (ev.type == ConnEvent::CE_SPAWN)
			{
				addConnection(ev.spawned);
			}
			else if (ev.type == ConnEvent::CE_CLOSE)
			{
				removeConnection(i);
				--i;
				continue;
			}

			_pfds[i].events = conn->events();
			_pfds[i].revents = 0;
		}
	}
}
