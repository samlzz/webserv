/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpDispatcher.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:19:40 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/20 21:29:23 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "HttpDispatcher.hpp"
#include "ft_log/LogOp.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/handlers/IHttpHandler.hpp"
#include "http/request/Cookies.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/routing/Router.hpp"
#include "log.h"
#include "utils/pathUtils.hpp"
#include "utils/stringUtils.hpp"

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
									const routing::Context &route,
									std::string &handlerName
								) const
{
	const Config::Server::Location	&loca = *route.location;

	if (loca.redirect)
	{
		handlerName = "RedirectHandler";
		return &_redirectHandler;
	}

	http::e_method	method = req.getMethod();
	if (method == http::MTH_GET || method == http::MTH_HEAD || method == http::MTH_POST)
	{
		std::string	cgiExt = path::subExt(path::subPath(route.normalizedUri));
		if (loca.cgiExts.find(cgiExt) != loca.cgiExts.end())
		{
			handlerName = "CgiHandler (ext: " + cgiExt + ")";
			return &_cgiHandler;
		}
	}
	
	if (method == http::MTH_GET || method == http::MTH_HEAD)
	{
		handlerName = "StaticHandler";
		return &_staticHandler;
	}

	if (method == http::MTH_DELETE)
	{
		handlerName = "DeleteHandler";
		return &_deleteHandler;
	}

	if (method == http::MTH_POST || method == http::MTH_PUT)
	{
		if (http::Data::getContentTypeKind(
			req.getField("Content-Type")
			) == http::CT_APPLICATION_X_WWW_FORM_URLENCODED
		)
		{
			handlerName = "FormHandler";
			return &_formHandler;
		}
		handlerName = "UploadHandler";
		return &_uploadHandler;
	}

	ft_log::log(WS_LOG_ROUTING, ft_log::LOG_WARN)
		<< "No handler found for request" << std::endl;
	return NULL;
}

static inline bool	_needToCloseConnection(http::e_status_code err)
{
	return (err == 400 || err == 408 || err == 411 || err == 413 || err == 414
			|| err == 431 || err >= 500);
}

ResponsePlan	HttpDispatcher::dispatch(
	const HttpRequest &req, const routing::Context &route
) const
{
	std::string			name;
	const IHttpHandler	*handler = findHandler(req, route, name);
	if (!handler)
		return ErrorBuilder::build(http::SC_NOT_IMPLEMENTED, route.location);
	ft_log::log(WS_LOG_ROUTING, ft_log::LOG_DEBUG)
		<< "Handler selected: " << name << std::endl;
	ResponsePlan		plan = handler->handle(req, route);

	if (req.getMethod() == http::MTH_HEAD && plan.body)
	{
		delete plan.body;
		plan.body = 0;
	}

	// ? Set Cookies
	plan.headers["Set-Cookie"] = req.getCookies().buildSetCookieHeaders();

	// ? Decide connection lifetime
	std::string	con = req.getField("Connection");
	str::lowerCase(con);
	if (_needToCloseConnection(plan.status)
		|| con.find("close") != std::string::npos
	)
		plan.headers["Connection"] = "close";
	else
		plan.headers["Connection"] = "keep-alive";

	plan.addStandardHeaders();
	return plan;
}