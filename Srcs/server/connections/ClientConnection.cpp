/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:55:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/09 14:38:21 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/poll.h>

#include "ClientConnection.hpp"
#include "config/Config.hpp"
#include "AConnection.hpp"
#include "ConnEvent.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

ClientConnection::ClientConnection(int cliSockFd, const Config::Server &config)
	: AConnection(cliSockFd), _conf(config)
{
	setNonBlocking();
}

// ============================================================================
// Methods
// ============================================================================

bool	ClientConnection::processIO(short revents)
{
/* TODO: Expected logic
	if ((revents & POLLIN) && !_request.done())
		_request.consume(_fd);

	if ((revents & POLLOUT) && _request.done())
		_response.produce(_fd);

	if (_response.done())
	{
		if (_response.shouldClose())
			return false;

		_request.reset();
		_response.reset();
	}
*/  (void)revents;
	return true;
}

ConnEvent	ClientConnection::handleEvents(short revents)
{
	if (isErrEvent(revents))
		return ConnEvent::close();

	if (!processIO(revents))
		return ConnEvent::close();

	return ConnEvent::none();
}