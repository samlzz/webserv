/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:19 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/07 18:07:37 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "Router.hpp"
#include "http/request/HttpRequest.hpp"
#include "server/ServerCtx.hpp"

namespace routing
{

// ============================================================================
// Construction
// ============================================================================

Context::Context(const ServerCtx &serv)
	: server(serv), location(0)
	, normalizedPath()
	, local(0), remote(0)
{}

// ============================================================================
// Routes resolving
// ============================================================================
/**
 * Remove /../ and /./ in path
 */ 
static inline std::string	_normalizeUri(const std::string &path)
{
	std::vector<std::string>	stack;
	std::string				token;
	bool					trailingSlash;

	if (path.empty() || path[0] != '/')
		return "/";

	trailingSlash = (path.size() > 1 && path[path.size() - 1] == '/');
// ? split
	std::istringstream	iss(path);
	while (std::getline(iss, token, '/'))
	{
		if (token.empty() || token == ".")
			continue;
		if (token == "..")
		{
			if (!stack.empty())
				stack.pop_back();
			continue;
		}
		stack.push_back(token);
	}

// ? join
	std::string result = "/";
	for (size_t i = 0; i < stack.size(); ++i)
	{
		result += stack[i];
		if (i + 1 < stack.size())
			result += '/';
	}
	if (trailingSlash && result.size() > 1)
		result += '/';

	return result;
}

Context	resolve(const HttpRequest &req,
				const ServerCtx &serv)
{
	Context ctx(serv);

	std::string uri = req.getPath(); // without query and fragment
	uri = _normalizeUri(uri);

	ctx.location = serv.config.findLocation(uri);
	if (ctx.location)
	{
		const std::string	&lp = ctx.location->path;

		// suffixe URI
		std::string suffix;
		if (lp != "/")
			suffix = uri.substr(lp.size());
		else
			suffix = uri;

		ctx.normalizedPath = lp + suffix;
	}
	
	return ctx;
}

} // namespace routing