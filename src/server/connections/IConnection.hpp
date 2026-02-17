/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConnection.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 12:13:50 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 16:58:20 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ICONNECTION_HPP__
# define __ICONNECTION_HPP__

# include <cstddef>
# include <sys/poll.h>

# include "ConnEvent.hpp"

// ============================================================================
// Interface class for poll-based connections
//
// An IConnection represents a single pollable entity associated with
// exactly one file descriptor.
//
// This interface is protocol-agnostic and transport-agnostic.
// ============================================================================
class IConnection {

private:

public:
	virtual ~IConnection() {};

	// ============================================================================
	// Poll integration
	// ============================================================================

	virtual int				fd(void) const = 0;
	virtual struct pollfd	pollFd(void) const = 0;

	virtual short			events(void) const = 0;
	virtual void			setEvents(short events) = 0;
	virtual void			addEvent(short event) = 0;

	virtual size_t			id(void) const = 0;

	virtual ConnEvent		checkTimeout(time_t now) = 0;
	/**
	 * Handle poll events reported by the Reactor.
	 *
	 * - Must not block
	 * - Must not call poll internally
	 * - Must perform at most one logical I/O step per call
	 *
	 * @param revents The events returned by poll()
	 * @return A ConnEvent describing the requested action
	 */
	virtual ConnEvent		handleEvents(short revents) = 0;

	virtual IConnection		*buddy(void) = 0;
	virtual void			detachBuddy(void) = 0;
};

#endif /* __ICONNECTION_HPP__ */