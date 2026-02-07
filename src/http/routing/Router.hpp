/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 15:17:51 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/07 18:07:33 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ROUTER_HPP__
# define __ROUTER_HPP__

# include <string>

# include "config/Config.hpp"
# include "server/AddrInfo.hpp"

// ============================================================================
// Forward declarations
// ============================================================================
class HttpRequest;
struct ServerCtx;

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
	const AddrInfo					*local;
	const AddrInfo					*remote;

	Context(const ServerCtx &serv);
private:
	Context();
};

Context	resolve(const HttpRequest &req, const ServerCtx &serv);

} // namespace routing

#endif /* __ROUTER_HPP__ */
