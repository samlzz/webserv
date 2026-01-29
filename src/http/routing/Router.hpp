/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 15:17:51 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 12:12:13 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ROUTER_HPP__
# define __ROUTER_HPP__

# include <string>
# include "config/Config.hpp"

// ============================================================================
// Forward declarations
// ============================================================================
class HttpRequest;

// ============================================================================
// Routes resolving
// ============================================================================
namespace routing
{

struct Context
{
	const Config::Server			&server;
	const Config::Server::Location	*location;
	std::string						normalizedPath;

	Context(const Config::Server &config);
private:
	Context();
};

Context	resolve(const HttpRequest &req, const Config::Server &config);

} // namespace rooting

#endif /* __ROUTER_HPP__ */
