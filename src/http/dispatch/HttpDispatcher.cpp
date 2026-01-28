/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpDispatcher.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:19:40 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 17:55:34 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "HttpDispatcher.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/handlers/IHttpHandler.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/routing/Router.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

HttpDispatcher::HttpDispatcher()
{}

HttpDispatcher::~HttpDispatcher()
{}

// ============================================================================
// Static helpers
// ============================================================================

static inline std::string	subExt(const std::string& pPath)
{
	std::string		result;
	size_t			start;

	if ((start = pPath.find_last_of('.')) == std::string::npos)
		return ("");

	result = pPath.substr(start + 1, pPath.length() - start);
	if (result.empty())
		return ("");

	return (result);
}

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
	if ((method == http::MTH_GET || method == http::MTH_POST)
		&& loca.cgiExts.find(subExt(route.normalizedPath)) != loca.cgiExts.end()
	)
		return &_cgiHandler;
	
	if (method == http::MTH_GET || method == http::MTH_HEAD)
		return &_staticHandler;

	if (method == http::MTH_DELETE)
		return &_deleteHandler;

	if (method == http::MTH_POST || method == http::MTH_PUT)
	{
		if (http::Data::getContentTypeKind(
			req.getHeader("Content-Type")
			) == http::CT_APPLICATION_X_WWW_FORM_URLENCODED
		)
			return &_formHandler;
		return &_uploadHandler;
	}

	return NULL;
}

ResponsePlan	HttpDispatcher::dispatch(
									const HttpRequest &req,
									const routing::Context &route
								) const
{
	if (!route.location)
		return ErrorBuilder::build(
			http::SC_NOT_FOUND,
			NULL
		);

	if (req.getState() == PARSING_ERROR)
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