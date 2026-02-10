/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCtx.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:34:00 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 12:47:48 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_CTX_HPP__
# define __SERVER_CTX_HPP__

# include "config/Config.hpp"
#include "http/Sessions.hpp"
# include "http/dispatch/HttpDispatcher.hpp"

// ============================================================================
// Runtime context for each server
// ============================================================================

struct ServerCtx {

	const HttpDispatcher	&dispatcher;
	const Config::Server	&config;
	mutable SessionsManager	sessions;

	ServerCtx(const HttpDispatcher &pDispatcher, const Config::Server &pConfig)
		: dispatcher(pDispatcher), config(pConfig), sessions()
	{}
};

#endif /* __SERVER_CTX_HPP__ */