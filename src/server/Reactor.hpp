/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 12:19:02 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/09 14:25:43 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __REACTOR_HPP__
# define __REACTOR_HPP__

# include <cstddef>
# include <stdint.h>
# include <sys/poll.h>
# include <vector>

# include "connections/IConnection.hpp"

# define POLL_TIMEOUT	-1

class Reactor {

private:
	std::vector<IConnection *>	_connections;
	std::vector<pollfd>			_pfds;
	bool						_running;

	void	removeConnection(size_t idx);

	// forbidden
	Reactor(const Reactor &other);
	Reactor& operator=(const Reactor &other);

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
};

#endif /* __REACTOR_HPP__ */
