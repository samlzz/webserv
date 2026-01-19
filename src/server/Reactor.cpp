/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:21:31 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/19 18:37:14 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <set>
#include <sys/wait.h>

#include "Reactor.hpp"
#include "server/Exceptions.hpp"
#include "server/connections/ConnEvent.hpp"
#include "server/connections/IConnection.hpp"

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

static inline void	_reapChildsProcess(void)
{
	while (waitpid(-1, 0, WNOHANG));
}

/**
 * Main poll listen loop
 */
void	Reactor::run(void)
{
	std::set<IConnection *>	pendingClose;

	_running = true;
	while (_running)
	{
		int ret = poll(_pfds.data(), _pfds.size(), POLL_TIMEOUT);
		if (ret < 0)
			throw SysError("poll");

		size_t	i = 0;
		while (i < _pfds.size())
		{
			IConnection	*conn = _connections[i];

			if (pendingClose.count(conn))
			{
				pendingClose.erase(conn);
				removeConnection(i);
				continue;
			}
			if (_pfds[i].revents == 0)
			{
				i++;
				continue;
			}

			ConnEvent	ev = conn->handleEvents(_pfds[i].revents);
			switch (ev.type)
			{
			case ConnEvent::CE_SPAWN:
				if (ev.conn)
					addConnection(ev.conn);
				break;

			case ConnEvent::CE_CLOSE_WITH:
				if (ev.conn && ev.conn != conn)
					pendingClose.insert(ev.conn);
				__attribute__ ((fallthrough));

			case ConnEvent::CE_CLOSE:
				removeConnection(i);
				continue;

			case ConnEvent::CE_NONE:
				__attribute__ ((fallthrough));
			default:
				break;
			}

			_pfds[i].events = conn->events();
			_pfds[i].revents = 0;
			i++;
			_reapChildsProcess();
		}
	}
}
