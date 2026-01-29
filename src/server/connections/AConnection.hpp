/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AConnection.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 12:44:32 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/24 15:07:20 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ACONNECTION_HPP__
# define __ACONNECTION_HPP__

# include <cerrno>
# include <sys/poll.h>

# include "IConnection.hpp"

class AConnection : public IConnection {

protected:
	int		_fd;
	short	_events;

public:
// ============================================================================
// Construction / Destruction
// ============================================================================

	AConnection(int fd, short events = POLLIN);
	virtual ~AConnection();

// ============================================================================
// Accessors
// ============================================================================

	virtual int			fd(void) const;
	virtual pollfd		pollFd(void) const;

	virtual short		events(void) const;
	virtual void		setEvents(short events);
	virtual void		addEvent(short event);

// ============================================================================
// Methods
// ============================================================================

	virtual ConnEvent	handleEvents(short revents) = 0;

	virtual ConnEvent	checkTimeout(time_t now);

	virtual IConnection	*buddy(void);
	virtual void		detachBuddy(void);

protected:
	void				setFdFlags(void);

private:

	// forbidden
	AConnection();
	AConnection(const AConnection& other);
	AConnection& operator=(const AConnection& other);
};

// ============================================================================
// Helpers
// ============================================================================

static inline bool	isErrEvent(short events)
{
	return events & (POLLERR | POLLNVAL | POLLHUP);
}

static inline bool	isNonBlockingErrno(void)
{
	return (errno == EAGAIN || errno == EWOULDBLOCK);
}

#endif /* __ACONNECTION_HPP__ */