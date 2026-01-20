/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 12:19:02 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/20 14:39:26 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __REACTOR_HPP__
# define __REACTOR_HPP__

# include <cstddef>
#include <set>
# include <stdint.h>
# include <sys/poll.h>
# include <vector>

# include "connections/IConnection.hpp"

# define POLL_TIMEOUT	-1

class Reactor {

private:
	std::vector<IConnection *>	_connections;
	std::vector<pollfd>			_pfds;
	std::set<IConnection *>		_pendingClose;
	bool						_running;

public:

// ============================================================================
// Construction / Destruction
// ============================================================================
	Reactor();
	~Reactor();

// ============================================================================
// Methods
// ============================================================================

	void	addConnection(IConnection *conn);
	void	run(void);
	void	stop(void);

private:

	void	removeConnection(size_t idx);
	bool	manageConnEvent(ConnEvent ev, size_t idx);

	// forbidden
	Reactor(const Reactor &other);
	Reactor& operator=(const Reactor &other);

};

#endif /* __REACTOR_HPP__ */
