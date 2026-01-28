/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCtx.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:34:00 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 12:35:09 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_CTX_HPP__
# define __SERVER_CTX_HPP__

# include "config/Config.hpp"
# include "http/dispatch/HttpDispatcher.hpp"

// ============================================================================
// Runtime context for each server
// ============================================================================

struct ServerCtx {

	const HttpDispatcher	&dispatcher;
	const Config::Server	&config;

	ServerCtx(const HttpDispatcher &pDispatcher, const Config::Server &pConfig)
		: dispatcher(pDispatcher), config(pConfig)
	{}
};

#endif /* __SERVER_CTX_HPP__ */