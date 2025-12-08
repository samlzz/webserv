/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 21:09:01 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/08 17:23:36 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <algorithm>
#include <cstddef>
#include <map>
#include <string>
#include <vector>

// ============================================================================
// Construction
// ============================================================================

Config::Server::Server()
	: host("0.0.0.0"), port(0), maxBodySize(0)
{}

Config::Server::Location::Location()
{}

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

std::string	Config::Server::Location::getErrorPage(
	const Server &parent,
	e_status_code code
) const
{
	const std::map<e_status_code, std::string>	&some = errorPages.getOr(
		parent.d_errorPages
	);
	std::map<e_status_code, std::string>::const_iterator it = some.find(code);

	if (it != some.end())
		return it->second;
	return "";
}

bool	Config::Server::Location::isAllowed(
	const Server &parent,
	e_method method
) const
{
	const std::vector<e_method>				&some = methods.getOr(
		parent.d_methods
	);
	std::vector<e_method>::const_iterator	it = std::find(
		some.begin(), some.end(), method
	);

	return (it != some.end());
}