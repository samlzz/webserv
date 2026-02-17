/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpDispatcher.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:19:40 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 19:56:03 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>

#include "HttpDispatcher.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/handlers/IHttpHandler.hpp"
#include "http/request/Cookies.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/routing/Router.hpp"
#include "utils/pathUtils.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

HttpDispatcher::HttpDispatcher()
{}

HttpDispatcher::~HttpDispatcher()
{}

// ============================================================================
// Methods
// ============================================================================

const IHttpHandler	*HttpDispatcher::findHandler(
									const HttpRequest &req,
									const routing::Context &route
								) const
{
	const Config::Server::Location	&loca = *route.location;

	if (loca.redirect)
		return &_redirectHandler;

	http::e_method	method = req.getMethod();
	if (method == http::MTH_GET || method == http::MTH_HEAD || method == http::MTH_POST)
	{
		std::string	cgiExt = path::subExt(path::subPath(route.normalizedPath));
		if (loca.cgiExts.find(cgiExt) != loca.cgiExts.end())
			return &_cgiHandler;
	}
	
	if (method == http::MTH_GET || method == http::MTH_HEAD)
		return &_staticHandler;

	if (method == http::MTH_DELETE)
		return &_deleteHandler;

	if (method == http::MTH_POST || method == http::MTH_PUT)
	{
		if (http::Data::getContentTypeKind(
			req.getField("Content-Type")
			) == http::CT_APPLICATION_X_WWW_FORM_URLENCODED
		)
			return &_formHandler;
		return &_uploadHandler;
	}

	return NULL;
}

ResponsePlan	HttpDispatcher::findPlan(
	const HttpRequest &req, const routing::Context &route
) const
{
	if (!route.location)
		return ErrorBuilder::build(
			http::SC_NOT_FOUND,
			NULL
		);

	if (req.isError())
		return ErrorBuilder::build(
			req.getStatusCode(),
			route.location
		);

	const Config::Server::Location& loca = *route.location;

	if (!loca.isMethodAllowed(req.getMethod()))
		return ErrorBuilder::build(
			http::SC_METHOD_NOT_ALLOWED,
			route.location
		);

	const IHttpHandler	*handler = findHandler(req, route);
	if (!handler)
		return ErrorBuilder::build(
			http::SC_NOT_IMPLEMENTED,
			route.location
		);

	return handler->handle(req, route);
}

ResponsePlan	HttpDispatcher::dispatch(
	const HttpRequest &req, const routing::Context &route
) const
{
	ResponsePlan		plan(findPlan(req, route));
	Cookies				&cookies = req.getCookies();
	std::istringstream	queryIss(req.getQuery());
	std::string			queryPart;
	
	while (std::getline(queryIss, queryPart, '&'))
	{
		size_t		pos = queryPart.find('=');
		std::string	key = queryPart.substr(0, pos);

		if (pos != std::string::npos
			&& route.location
			&& route.location->isCookiesSet(key)
		)
			cookies.setCookie(key, queryPart.substr(pos + 1));
	}
	plan.headers["Set-Cookie"] = cookies.buildSetCookieHeaders();
	plan.headers["Connection"] = "keep-alive";

	http::e_status_code code = plan.status;
	if (code == 400 || code == 408 || code == 411 || code == 413 || code ==  414
		|| code == 431 || code >= 500)
	{
		plan.headers["Connection"] = "close";
		return plan;
	}

	if (req.hasField("Connection") && req.getField("Connection") == "close") 
	{
		plan.headers["Connection"] = "close";
		return plan;
	}

	return plan;
}