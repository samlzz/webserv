/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpDispatcher.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:19:40 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/31 12:28:29 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "HttpDispatcher.hpp"
#include "ft_log/LogOp.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/handlers/IHttpHandler.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/routing/Router.hpp"
#include "log.h"
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
	{
		ft_log::log(WS_LOG_CLI_ROUTING, ft_log::LOG_DEBUG)
			<< "Handler selected: RedirectHandler" << std::endl;
		return &_redirectHandler;
	}

	http::e_method	method = req.getMethod();
	if (method == http::MTH_GET || method == http::MTH_HEAD || method == http::MTH_POST)
	{
		std::string	cgiExt = path::subExt(path::subPath(route.normalizedPath));
		if (loca.cgiExts.find(cgiExt) != loca.cgiExts.end())
		{
			ft_log::log(WS_LOG_CLI_ROUTING, ft_log::LOG_DEBUG)
				<< "Handler selected: CgiHandler (ext: " << cgiExt << ")" << std::endl;
			return &_cgiHandler;
		}
	}
	
	if (method == http::MTH_GET || method == http::MTH_HEAD)
	{
		ft_log::log(WS_LOG_CLI_ROUTING, ft_log::LOG_DEBUG)
			<< "Handler selected: StaticHandler" << std::endl;
		return &_staticHandler;
	}

	if (method == http::MTH_DELETE)
	{
		ft_log::log(WS_LOG_CLI_ROUTING, ft_log::LOG_DEBUG)
			<< "Handler selected: DeleteHandler" << std::endl;
		return &_deleteHandler;
	}

	if (method == http::MTH_POST || method == http::MTH_PUT)
	{
		if (http::Data::getContentTypeKind(
			req.getField("Content-Type")
			) == http::CT_APPLICATION_X_WWW_FORM_URLENCODED
		)
		{
			ft_log::log(WS_LOG_CLI_ROUTING, ft_log::LOG_DEBUG)
				<< "Handler selected: FormHandler" << std::endl;
			return &_formHandler;
		}
		ft_log::log(WS_LOG_CLI_ROUTING, ft_log::LOG_DEBUG)
			<< "Handler selected: UploadHandler" << std::endl;
		return &_uploadHandler;
	}

	ft_log::log(WS_LOG_CLI_ROUTING, ft_log::LOG_WARN)
		<< "No handler found for request" << std::endl;
	return NULL;
}

ResponsePlan	HttpDispatcher::dispatch(
									const HttpRequest &req,
									const routing::Context &route
								) const
{
	if (!route.location)
	{
		ft_log::log(WS_LOG_CLI_ROUTING, ft_log::LOG_ERROR)
			<< "No location found for: " << req.getPath() << std::endl;
		return ErrorBuilder::build(
			http::SC_NOT_FOUND,
			NULL
		);
	}

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