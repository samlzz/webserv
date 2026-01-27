/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlersRegistry.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:22:12 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 20:30:34 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HandlersRegistry.hpp"
#include <stdexcept>

// ============================================================================
// Construction / Destruction
// ============================================================================

HandlersRegistry::HandlersRegistry()
{}

HandlersRegistry::~HandlersRegistry()
{}

// ============================================================================
// Methods
// ============================================================================

void	HandlersRegistry::registerHandler(
	e_handler_kind key,
	const IHttpHandler* handler
)
{
	if (!handler)
		throw std::runtime_error("HandlerRegistry: null handler");

	_registry[key] = handler;
}

const IHttpHandler*	HandlersRegistry::getHandler(e_handler_kind key) const
{
	t_handlers::const_iterator	it;
	
	it = _registry.find(key);
	if (it == _registry.end())
		return 0;
	return it->second;
}