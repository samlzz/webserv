/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpDispatcher.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:19:40 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 16:55:10 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>

#include "HttpDispatcher.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/handlers/IHttpHandler.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/routing/Router.hpp"
#include "utils/fileSystemUtils.hpp"

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

	std::istringstream	queryIss(req.getQuery());
	std::string			queryPart;
	
	// ? Set cookie
	while (std::getline(queryIss, queryPart, '&'))
	{
		size_t		pos = queryPart.find('=');
		std::string	key = queryPart.substr(0, pos);
		std::string	val = pos != std::string::npos
							? queryPart.substr(pos + 1)
							: "";
		if (loca.isCookiesSet(key))
			req.getCookies().setCookie(key, val);
	}

	if (loca.redirect)
		return &_redirectHandler;

	http::e_method	method = req.getMethod();
	if ((method == http::MTH_GET || method == http::MTH_POST)
		&& loca.cgiExts.find(fs::subExt(route.normalizedPath)) != loca.cgiExts.end()
	)
		return &_cgiHandler;
	
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
	if (!handler) // ? sould never happend cause of previous if
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
	ResponsePlan	plan(findPlan(req, route));
	plan.headers["Set-Cookie"] = req.getCookies().buildSetCookieHeaders();
	return plan;
}