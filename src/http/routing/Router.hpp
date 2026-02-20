/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 15:17:51 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/20 11:58:11 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ROUTER_HPP__
# define __ROUTER_HPP__

# include <string>

# include "config/Config.hpp"
# include "http/Sessions.hpp"

// ============================================================================
// Forward declarations
// ============================================================================
class Cookies;
struct AddrInfo;

// ============================================================================
// Routes resolving
// ============================================================================
namespace routing
{

struct Context
{
	const Config::Server::Location		*location;
	std::string							normalizedUri;
	mutable SessionsManager::Session	*currSession;
	const AddrInfo						*local;
	const AddrInfo						*remote;

	Context();
};

Context	resolve(const std::string &uri,
				const Config::Server &config,
				Cookies *store);

} // namespace routing

#endif /* __ROUTER_HPP__ */
