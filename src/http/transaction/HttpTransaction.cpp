/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpTransaction.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 11:48:06 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/20 22:53:07 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <cstdlib>
#include <sstream>
#include <string>

#include "HttpTransaction.hpp"
#include "config/Config.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/request/Cookies.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/routing/Router.hpp"
#include "server/AddrInfo.hpp"
#include "server/ServerCtx.hpp"
#include "utils/Optionnal.hpp"
#include "utils/stringUtils.hpp"
#include "utils/urlUtils.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

HttpTransaction::HttpTransaction(ServerCtx &serv,
							const AddrInfo &remote,
							const AddrInfo &local)
	: _route(), _ctx(serv), _remote(remote), _local(local)
{}

HttpTransaction::~HttpTransaction()
{}

// ============================================================================
// Accessor
// ============================================================================

bool	HttpTransaction::isHeadersValidated(void) const
{
	return _route.isSome();
}

const Config::Server::Location	*
		HttpTransaction::getLocation(void) const
{
	if (_route.isSome())
		return (*_route).location;
	return NULL;
}

void	HttpTransaction::reset(void)
{
	_route.reset();
}

// ============================================================================
// Helper
// ============================================================================

static inline bool	_parseContentLength(const std::string& s, size_t& out)
{
	if (s.empty())
		return false;
	size_t	value = 0;

	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] < '0' || s[i] > '9')
			return false;
		size_t digit = s[i] - '0';
		if (value > (SIZE_MAX - digit) / 10)
			return false;

		value = value * 10 + digit;
	}

	out = value;
	return true;
}

// ============================================================================
// ResponsePlan generator
// ============================================================================

ResponsePlan	HttpTransaction::onParsingError(const HttpRequest &req)
{
	const Config::Server::Location	*tmp = NULL;

	if (_route.isSome())
		tmp = (*_route).location;
	else if (!req.getPath().empty())
		tmp = routing::resolve(req.getPath(), _ctx.config, 0).location;

	ResponsePlan	plan = ErrorBuilder::build(req.getStatusCode(), tmp);
	if (req.hasBody() && !req.isBodyComplete())
		plan.headers["Connection"] = "close";
	return plan;
}

Optionnal<ResponsePlan>	HttpTransaction::onHeadersComplete(HttpRequest &req)
{
	_route = routing::resolve(req.getPath(), _ctx.config, &req.getCookies());
	routing::Context &r = *_route;
	r.local = &_local;
	r.remote = &_remote;

	bool					hasBody = req.hasBody();
	Optionnal<ResponsePlan>	plan = checkHeaders(req, r.location);
	if (plan.isSome())
	{
		if (hasBody)
			(*plan).headers["Connection"] = "close";
		return *plan;
	}

	handleSession(req.getCookies());
	setCookies(req.getCookies(), req.getQuery());

	// ? Wait for body to produce ResponsePlan
	if (hasBody)
	{
		req.setBodySize(r.location->maxBodySize);
		return Optionnal<ResponsePlan>();
	}
	// ? Respond now
	return _ctx.dispatcher.dispatch(req, r);
}

ResponsePlan	HttpTransaction::onBodyComplete(const HttpRequest &req) const
{
	// ? Should never be called before onHeadersComplete
	if (_route.isNone())
		return ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, 0);
	return _ctx.dispatcher.dispatch(req, *_route);
}

// ============================================================================
// Private Methods
// ============================================================================

Optionnal<ResponsePlan>	HttpTransaction::checkHeaders(HttpRequest &req, const Config::Server::Location *loc)
{
	if (!req.hasField("Host"))
		return ErrorBuilder::build(http::SC_BAD_REQUEST, loc);

	if (!loc)
		return ErrorBuilder::build(http::SC_NOT_FOUND, 0);

	if (!loc->isMethodAllowed(req.getMethod()))
		return ErrorBuilder::build(
			http::SC_METHOD_NOT_ALLOWED,
			loc
		);
	
	http::e_method	m = req.getMethod();
	if (m == http::MTH_POST || m == http::MTH_PUT)
	{
		if (req.hasField("Transfer-Encoding"))
		{
			if (req.hasField("Content-Length"))
				return ErrorBuilder::build(http::SC_BAD_REQUEST, loc);
			std::string te = str::trim(req.getField("Transfer-Encoding"));
			if (str::lowerCase(te) != "chunked")
				return ErrorBuilder::build(http::SC_NOT_IMPLEMENTED, loc);
		}
		else if (req.hasField("Content-Length"))
		{
			size_t		clNb;
			if (!_parseContentLength(req.getField("Content-Length"), clNb))
				return ErrorBuilder::build(http::SC_BAD_REQUEST, loc);
			if (clNb > loc->maxBodySize)
				return ErrorBuilder::build(http::SC_CONTENT_TOO_LARGE, loc);
			req.setContentLength(clNb);
		}
		else
			return ErrorBuilder::build(http::SC_LENGTH_REQUIRED, loc);
	}
	return Optionnal<ResponsePlan>();
}

void	HttpTransaction::handleSession(Cookies &store)
{
	std::string	sessionId = store.getCookie("sessionId");

	_ctx.sessions.clearExpiredSessions();
	if (sessionId.empty() || !_ctx.sessions.sessionExists(sessionId))
	{
		sessionId = _ctx.sessions.createSession("guest");
		store.setCookie("sessionId", sessionId);
	}
	SessionsManager::Session *session = &_ctx.sessions.getSession(sessionId);
	session->updateActivity();
	if (_route.isSome())
		(*_route).currSession = session;
}

void	HttpTransaction::setCookies(Cookies &store, const std::string &query) const
{
	std::istringstream	queryIss(query);
	std::string			queryPart;
	
	while (std::getline(queryIss, queryPart, '&'))
	{
		size_t		pos = queryPart.find('=');
		std::string	key = url::decode(queryPart.substr(0, pos));

		if (pos != std::string::npos
			&& _route
			&& (*_route).location->isCookiesSet(key)
		)
		{
			std::string	val = url::decode(queryPart.substr(pos + 1));
			size_t		end = val.find("\r\n");
			store.setCookie(key, val.substr(0, end));
		}
	}
}