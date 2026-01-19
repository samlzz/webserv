/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnEvent.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 10:58:03 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/17 11:35:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CONNEVENT_HPP__
# define __CONNEVENT_HPP__

// ============================================================================
// Forward declarations
// ============================================================================
class IConnection;

// ============================================================================
// Connections event structure
// ============================================================================
struct ConnEvent {

	enum e_type
	{
		CE_NONE,
		CE_CLOSE,
		CE_CLOSE_WITH,
		CE_SPAWN
	};

	enum e_type	type;
	IConnection	*conn;

	// ============================================================================
	// Factories
	// ============================================================================

	static ConnEvent none()						{ return ConnEvent(CE_NONE, 0); }
	static ConnEvent close()					{ return ConnEvent(CE_CLOSE, 0); }
	static ConnEvent closeWith(IConnection *c)	{ return ConnEvent(CE_CLOSE_WITH, c); }
	static ConnEvent spawn(IConnection *c)		{ return ConnEvent(CE_SPAWN, c); }

private:
	ConnEvent(e_type t, IConnection *c)
		: type(t), conn(c) {}
};

#endif /* __CONNEVENT_HPP__ */