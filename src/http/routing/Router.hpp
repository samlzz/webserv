/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 15:17:51 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 12:50:25 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ROUTER_HPP__
# define __ROUTER_HPP__

# include <string>

# include "config/Config.hpp"
#include "http/Cookies.hpp"
#include "http/Sessions.hpp"

// ============================================================================
// Forward declarations
// ============================================================================
class HttpRequest;
struct ServerCtx;
struct Session;
// ============================================================================
// Routes resolving
// ============================================================================
namespace routing
{

struct Context
{
	const ServerCtx					&server;
	const Config::Server::Location	*location;
	std::string						normalizedPath;
	Cookies							cookies;
	SessionsManager::Session		*session;

	Context(const ServerCtx &serv)
		: server(serv), location(0), normalizedPath(), cookies(), session(0)
	{}
private:
	Context();
};

Context	resolve(const HttpRequest &req, const ServerCtx &serv);

} // namespace rooting

#endif /* __ROUTER_HPP__ */
