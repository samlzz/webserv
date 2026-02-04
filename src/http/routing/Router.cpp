/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 12:00:19 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 12:50:43 by sliziard         ###   ########.fr       */
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

static inline std::string _trailingSlash(const std::string &path)
{
	std::string result;
	
	for (size_t i = 0; i < path.length(); ++i)
	{
		if (path[i] != '/' || (i == 0 || path[i - 1] != '/'))
		{
			result += path[i];
		}
	}

	return (result);
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
	ctx.normalizedPath = _trailingSlash(ctx.normalizedPath);
	
	//parse cookies
	ctx.cookies.parseCookies(req.getHeaders());

	//manage sessions
	std::string sessionId = ctx.cookies.getCookie("sessionId");

	serv._sessions.clearExpiredSessions(SESSION_TIMEOUT);
	if (sessionId.empty() || !serv._sessions.sessionExists(sessionId))
	{
		sessionId = serv._sessions.createSession("guest");
		ctx.cookies.setCookie("sessionId", sessionId);
	}
	SessionsManager::Session *session = &serv._sessions.getSession(sessionId);
	session->last_activity = std::time(0);
	ctx.session = session;

	return ctx;
}

} // namespace routing