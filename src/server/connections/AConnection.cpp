/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AConnection.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 12:49:52 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/09 14:36:24 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdint.h>
#include <sys/poll.h>
#include <unistd.h>

#include "AConnection.hpp"
#include "server/Exceptions.hpp"

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

void	AConnection::setNonBlocking(void)
{
	int32_t flags = fcntl(_fd, F_GETFL, 0);
	if (flags == -1 || fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw SysError("fcntl");
}