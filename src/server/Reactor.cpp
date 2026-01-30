/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:21:31 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/30 16:24:49 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <ctime>
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
	while (waitpid(-1, 0, WNOHANG) > 0);
}

bool	Reactor::manageConnEvent(ConnEvent ev, size_t idx)
{
	switch (ev.type)
	{
	case ConnEvent::CE_SPAWN:
	{
		addConnection(ev.conn);
		IConnection	*buddy = ev.conn->buddy();
		while (buddy)
		{
			addConnection(buddy);
			buddy = buddy->buddy();
		}
		break;
	}

	case ConnEvent::CE_CLOSE:
	{
		IConnection	*victim = _connections[idx];
		IConnection	*buddy = victim->buddy();

		if (buddy)
		{
			victim->detachBuddy();
			if (buddy->buddy() == victim)
				buddy->detachBuddy();
			_pendingClose.insert(buddy);
		}
		removeConnection(idx);
		return true;
	}

	case ConnEvent::CE_REFRESH:
	{
		_pfds[idx].events = _connections[idx]->events();
		_pfds[idx].revents = 0;
		break;
	}

	case ConnEvent::CE_NONE:
		__attribute__ ((fallthrough));
	default:
		break;
	}
	return false;
}

/**
 * Main poll listen loop
 */
void	Reactor::run(void)
{
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

			if (_pendingClose.count(conn))
			{
				_pendingClose.erase(conn);
				removeConnection(i);
				continue;
			}
			if (manageConnEvent(conn->checkTimeout(std::time(0)), i))
				continue;
			if (_pfds[i].revents == 0)
			{
				i++;
				continue;
			}
			if (manageConnEvent(conn->handleEvents(_pfds[i].revents), i))
				continue;
			
			_pfds[i].events = conn->events();
			_pfds[i].revents = 0;
			i++;
		}
		_reapChildsProcess();
	}
}
