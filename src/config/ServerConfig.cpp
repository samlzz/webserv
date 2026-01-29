/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 21:09:01 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 14:06:55 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <string>
#include <vector>

#include "Config.hpp"
#include "http/HttpTypes.hpp"

// ============================================================================
// Server defaults values
// ============================================================================
const char				*Config::ServerDefaults::host = "0.0.0.0";
const uint16_t			Config::ServerDefaults::port = 80;
const size_t			Config::ServerDefaults::maxBodySize = 1 << 20; // 1MB

const http::e_method	Config::ServerDefaults::methods[3] = 
	{ http::MTH_GET, http::MTH_POST, http::MTH_DELETE };

const char				*Config::ServerDefaults::root = "html";
const char				*Config::ServerDefaults::index = "index.html";
const bool				Config::ServerDefaults::autoindex = false;
const char				*Config::ServerDefaults::defaultErrPage = "/errors/default.html";

// ============================================================================
// Operator
// ============================================================================

// reverse comp to sort in descending order
bool	Config::Server::Location::operator<(const Location &other) const
{
	return path.length() > other.path.length();
}

// ============================================================================
// Accessors
// ============================================================================

// longest prefix match (location was already sorted by decreasing length)
const Config::Server::Location	*
Config::Server::findLocation(const std::string &path) const
{
	for (size_t i = 0; i < locations.size(); ++i)
	{
		const Location		&loc = locations[i];
		const std::string	&lp = loc.path;

		// always last location bc of pre sort
		if (lp == "/")
			return &loc;

		if (
			path.compare(0, lp.size(), lp) == 0
			&& (path.size() == lp.size() || path[lp.size()] == '/')
		)
			return &loc;
	}
	return 0;
}

bool	Config::Server::Location::isMethodAllowed(http::e_method method) const
{
	for (size_t i = 0; i < methods.size(); ++i)
	{
		if (method == methods[i])
			return true;
	}
	return false;
}