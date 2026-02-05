/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AConnection.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 12:49:52 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/01 20:41:11 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdint.h>
#include <sys/poll.h>
#include <unistd.h>

#include "AConnection.hpp"
#include "server/Exceptions.hpp"
#include "server/connections/ConnEvent.hpp"
#include "server/connections/IConnection.hpp"

// ============================================================================
// Construction
// ============================================================================

AConnection::AConnection(): _fd(-1), _events(0) {}

AConnection::AConnection(int fd, short events)
	: _fd(fd), _events(events)
{
	if (_fd < 0)
		throw SysError("invalid socket fd");
}

// ============================================================================
// Destruction
// ============================================================================

AConnection::~AConnection()
{
	if (_fd >= 0)
		close(_fd);
}

// ============================================================================
// Accessors
// ============================================================================

int		AConnection::fd(void) const				{ return _fd; }
pollfd	AConnection::pollFd(void) const
{
	return (struct pollfd){_fd, _events, 0};
}

short	AConnection::events(void) const			{ return _events; }
void	AConnection::setEvents(short events)	{ _events = events; }
void	AConnection::addEvent(short event)		{ _events |= event; }

// ============================================================================
// Methods
// ============================================================================

void	AConnection::setFdFlags(void)
{
	int32_t flags;
	
	flags = fcntl(_fd, F_GETFL);
	if (flags == -1
		|| fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1
	)
		throw SysError("fcntl");

	flags = fcntl(_fd, F_GETFD);
	if (flags == -1
		|| fcntl(_fd, F_SETFD, flags | FD_CLOEXEC) == -1
	)
		throw SysError("fcntl");
}

ConnEvent	AConnection::checkTimeout(time_t now)
{
	(void)now;
	return ConnEvent::none();
}

IConnection	*AConnection::buddy(void)		{ return 0; }
void		AConnection::detachBuddy(void)	{}