/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 21:09:01 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/10 02:31:56 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <string>
#include <vector>

#include "Config.hpp"

// ============================================================================
// Construction
// ============================================================================

Config::Server::Server()
	: host("0.0.0.0"), port(80)
	, maxBodySize(1 << 20) // 1MB
	, d_methods(1, http::MTH_GET)
	, d_root("html")
	, d_index("index.html")
	, d_autoindex(false)
{}

Config::Server::Location::Location()
{}

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

const Config::Server::Location	*
Config::Server::findLocation(const std::string &path) const
{
	const Location	*best = 0;
	size_t			bestLen = 0;

	for (size_t i = 0; i < locations.size(); ++i)
	{
		const std::string& locPath = locations[i].path;
		if (path.compare(0, locPath.length(), locPath) == 0
			&& locPath.length() > bestLen)
		{
			best = &locations[i];
			bestLen = locPath.length();
		}
	}
	return best;
}