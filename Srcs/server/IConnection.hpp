/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConnection.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 12:13:50 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/09 11:45:22 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ICONNECTION_HPP__
# define __ICONNECTION_HPP__

# include <sys/poll.h>

# include "server/ConnEvent.hpp"

// ============================================================================
// Interface class for poll-based connections
// ============================================================================
class IConnection {

private:

public:
	virtual ~IConnection() {};

	virtual int				fd(void) const = 0;
	virtual struct pollfd	pollFd(void) const = 0;

	virtual short			events(void) const = 0;
	virtual void			setEvents(short events) = 0;
	virtual void			addEvent(short event) = 0;

	virtual ConnEvent		handleEvents(short revents) = 0;
};

#endif /* __ICONNECTION_HPP__ */