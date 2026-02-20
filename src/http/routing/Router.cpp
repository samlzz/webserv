/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:19 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/20 11:58:31 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "Router.hpp"
#include "config/Config.hpp"
#include "http/request/Cookies.hpp"
#include "utils/fileSystemUtils.hpp"

namespace routing
{

// ============================================================================
// Construction
// ============================================================================

Context::Context()
	: location(0)
	, normalizedUri()
	, currSession(0)
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
	if (trailingSlash && result.size() > 1 && result[result.size() - 1] != '/')
		result += '/';

	return result;
}

static inline std::string	_prefixExtension(const std::string &path,
											const std::string &prefix)
{
	size_t		extPos = path.find_last_of('.');
	std::string	result = path.substr(0, extPos) + prefix;

	if (extPos != std::string::npos)
		result += path.substr(extPos);

	return result;
}

static inline std::string	_uriToPath(const std::string &uri,
									const std::string &locationPath)
{
	std::string	relative = uri.substr(locationPath.size());

	if (relative.empty())
		return "/";

	if (relative[0] != '/')
		relative = "/" + relative;
	return relative;
}

Context	resolve(const std::string &uri, const Config::Server &config, Cookies *store)
{
	Context ctx;

	ctx.location = config.findLocation(uri);
	if (!ctx.location)
		return ctx;

	const std::string	&lp = ctx.location->path;
	ctx.normalizedUri = _uriToPath(_normalizeUri(uri), lp);

	if (!store)
		return ctx;

	// ? Serve files dynamically from cookies
	const std::vector<std::string>	&cookiesVary = ctx.location->cookiesVary;
	for (size_t i = 0; i < cookiesVary.size(); ++i)
	{
		std::string cookieValue = store->getCookie(cookiesVary[i]);
		if (cookieValue.empty())
			continue;
		std::string	newPath = _prefixExtension(
					ctx.normalizedUri, "_" + cookieValue);
		if (fs::isExist(ctx.location->root + newPath))
		{
			ctx.normalizedUri = newPath;
			break;
		}
	}

	return ctx;
}

} // namespace routing