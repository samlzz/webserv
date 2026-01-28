/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:49:38 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 11:02:05 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config/Config.hpp"
#include "ErrorHandler.hpp"
#include "config/validation/configValidate.hpp"
#include "http/HttpData.hpp"
#include "http/handlers/bodySrcs/FileBodySource.hpp"
#include "http/handlers/bodySrcs/MemoryBodySource.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/routing/Router.hpp"

ErrorHandler::ErrorHandler()
{}

ResponsePlan	ErrorHandler::handle(const HttpRequest &req, const routing::Context &route)
{
	return build(req.getStatusCode(), route.location);
}

ResponsePlan	ErrorHandler::build(
	http::e_status_code status,
	const Config::Server::Location *location
)
{
	if (location)
	{
		Config::t_errPages::const_iterator it = location->errorPages.find(status);
		if (it != location->errorPages.end())
			return buildFromErrorPage(status, it->second);

		if (!location->defaultErrPage.empty())
			return buildFromErrorPage(status, location->defaultErrPage);
	}
	return buildDefault(status);
}

ResponsePlan	ErrorHandler::buildDefault(http::e_status_code status)
{
	std::string			reason = http::Data::getStatusType(status);
	std::ostringstream	stream;
	stream	<< "<!DOCTYPE html>"
			<< "<html>\r\n"
			<< "<head><title>" << status << " " << reason << "</title></head>\r\n"
			<< "<body>\r\n"
			<< "<h1>" << status << " " << reason << "</h1>\r\n"
			<< "<p> An error has occured. </p>\r\n"
			<< "</body>\r\n"
			<< "</html>\r\n";
	std::string			body = stream.str();
	ResponsePlan		plan;

	plan.status = status;
	plan.headers["Content-Type"] = "text/html";
	plan.headers["Content-Length"] = toString(body.length());
	plan.body = new MemoryBodySource(body);

	return plan;
}

ResponsePlan	ErrorHandler::buildFromErrorPage(
	http::e_status_code status,
	const std::string& path
)
{
	struct stat	st;
	if (stat(path.c_str(), &st))
		return buildDefault(status);
	int fd = open(path.c_str(), O_RDONLY);

	if (fd < 0)
		return buildDefault(status);

	ResponsePlan plan;
	plan.status = status;
	plan.headers["Content-Type"] = "text/html";
	plan.headers["Content-Length"] = toString(st.st_size);
	plan.body = new FileBodySource(fd);

	return plan;
}