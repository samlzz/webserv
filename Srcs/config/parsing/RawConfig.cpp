/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RawConfig.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 16:28:06 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/29 07:48:22 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

#include "config/Config.hpp"
#include "configParse.hpp"
#include "RawConfig.hpp"

namespace config_parse
{

RawServer::RawLocation::RawLocation(const std::string &pPath): path(pPath) {}

Config::Server::Location	RawServer::RawLocation::normalize(
	const RawServer &parent,
	const Config::ServerDefaults &def
)
{
	Config::Server::Location out;
	out.path = path;

	std::vector<http::e_method>	parentMethods;
	if (parent.d_methods.empty())
		parentMethods = std::vector<http::e_method>(def.methods, def.methods + 3);
	else
		parentMethods = parent.d_methods;
	std::string		parentRoot = parent.d_root.getOr(def.root);
	std::string		parentIndex = parent.d_index.getOr(def.index);
	bool			parentAutoindex = parent.d_autoindex.getOr(def.autoindex);
	std::string		parentDefaultErr = parent.d_defaultErrPage.getOr(
															def.defaultErrPage);

	out.methods = methods.empty() ? parentMethods : methods;
	out.root = root.getOr(parentRoot);
	out.index = index.getOr(parentIndex);
	out.autoindex = autoindex.getOr(parentAutoindex);

	out.errorPages = concatMap(parent.d_errorPages, errorPages);
	out.defaultErrPage = defaultErrPage.getOr(parentDefaultErr);
	if (uploadPath)
		out.cgiExts = cgiExts;
	else
		out.cgiExts = concatMap(parent.d_cgiExts, cgiExts);

	out.uploadPath = uploadPath;
	out.redirect = redirect;

	return out;
}

static inline void	_setHost(struct in_addr *dest, std::string hostStr)
{
	if (hostStr == "localhost")
		hostStr = "127.0.0.1";
	if (!inet_pton(AF_INET, hostStr.c_str(), dest))
		throw ValueError("host IPv4", hostStr);
}

Config::Server	RawServer::normalize(const Config::ServerDefaults &def)
{
	Config::Server out;

	_setHost(&out.host, host.getOr(def.host));
	out.port = port.getOr(def.port);
	out.maxBodySize = maxBodySize.getOr(def.maxBodySize);

	if (locations.empty())
		locations.push_back(RawLocation("/"));
	for (size_t i = 0; i < locations.size(); ++i)
	{
		out.locations.push_back(
			locations[i].normalize(*this, def)
		);
	}
	std::sort(out.locations.begin(), out.locations.end());
	return out;
}

} // namespace config_parse