/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 15:17:51 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/10 14:19:31 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ROUTER_HPP__
# define __ROUTER_HPP__

# include <string>

# include "config/Config.hpp"
# include "http/Sessions.hpp"
# include "server/AddrInfo.hpp"

#define SESSION_TIMEOUT 1800 // 30 minutes
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
	const ServerCtx						&server;
	const Config::Server::Location		*location;
	std::string							normalizedPath;
	mutable SessionsManager::Session	*currSession;
	const AddrInfo						*local;
	const AddrInfo						*remote;

	Context(const ServerCtx &serv);
private:
	Context();
};

Context	resolve(const HttpRequest &req, const ServerCtx &serv);

} // namespace routing

#endif /* __ROUTER_HPP__ */
