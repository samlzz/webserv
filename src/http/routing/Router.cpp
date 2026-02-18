/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:19 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/18 13:12:19 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "Router.hpp"
#include "ft_log/LogOp.hpp"
#include "ft_log/level.hpp"
#include "http/request/Cookies.hpp"
#include "http/request/HttpRequest.hpp"
#include "log.h"
#include "server/ServerCtx.hpp"
#include "utils/fileSystemUtils.hpp"

namespace routing
{

// ============================================================================
// Construction
// ============================================================================

Context::Context(const ServerCtx &serv)
	: server(serv), location(0)
	, normalizedPath()
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

static inline std::string	_prefixExtension(const std::string &path, const std::string &prefix)
{
	size_t		extPos = path.find_last_of('.');
	std::string	result = path.substr(0, extPos) + prefix;

	if (extPos != std::string::npos)
		result += path.substr(extPos);

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
		ft_log::log(WS_LOG_ROUTING, ft_log::LOG_DEBUG)
			<< "Matched location " << lp << std::endl;

		// suffixe URI
		std::string suffix;
		if (lp != "/")
			suffix = uri.substr(lp.size());
		else
			suffix = uri;

		ctx.normalizedPath = lp + suffix;

		// ? Serve files dynamically from cookies
		const std::vector<std::string>	&cookiesVary = ctx.location->cookiesVary;
		for (size_t i = 0; i < cookiesVary.size(); ++i)
		{
			std::string cookieValue = req.getCookies().getCookie(cookiesVary[i]);
			if (cookieValue.empty())
				continue;
			std::string	newPath = _prefixExtension(
						ctx.normalizedPath, "_" + cookieValue);
			if (fs::isExist(ctx.location->root + newPath))
			{
				ctx.normalizedPath = newPath;
				break;
			}
		}
	}
	else
		ft_log::log(WS_LOG_ROUTING, ft_log::LOG_ERROR)
			<< "No location found for: " << req.getPath() << std::endl;

	// manage sessions
	Cookies		&cookies = req.getCookies();
	std::string	sessionId = cookies.getCookie("sessionId");

	serv.sessions.clearExpiredSessions(SESSION_TIMEOUT);
	if (sessionId.empty() || !serv.sessions.sessionExists(sessionId))
	{
		sessionId = serv.sessions.createSession("guest");
		cookies.setCookie("sessionId", sessionId);
	}
	SessionsManager::Session *session = &serv.sessions.getSession(sessionId);
	session->updateActivity();
	ctx.currSession = session;

	return ctx;
}

} // namespace routing