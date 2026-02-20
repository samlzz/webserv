/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorBuilder.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:49:38 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/20 22:45:29 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <fcntl.h>
#include <string>
#include <unistd.h>

#include "config/Config.hpp"
#include "ErrorBuilder.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/handlers/bodySrcs/FileBodySource.hpp"
#include "http/handlers/bodySrcs/MemoryBodySource.hpp"
#include "http/response/ResponsePlan.hpp"
#include "utils/fileSystemUtils.hpp"
#include "utils/stringUtils.hpp"

ResponsePlan	ErrorBuilder::getResponsePlan(
	http::e_status_code status,
	const Config::Server::Location *location
)
{
	if (location)
	{
		Config::t_errPages::const_iterator it = location->errorPages.find(status);
		if (it != location->errorPages.end())
		{
			return buildFromErrorPage(status, it->second, location->root);
		}

		if (!location->defaultErrPage.empty())
			return buildFromErrorPage(status, location->defaultErrPage, location->root);
	}
	return buildDefault(status);
}

ResponsePlan	ErrorBuilder::build(
	http::e_status_code status,
	const Config::Server::Location *location
)
{
	ResponsePlan	result(getResponsePlan(status, location));

	if (status == http::SC_METHOD_NOT_ALLOWED)
	{
		std::string	allow;
		for (size_t i = 0; i < location->methods.size(); ++i)
		{
			allow += str::toString(location->methods[i]);
			if (i < location->methods.size() - 1)
				allow += ", ";
		}
		result.headers["Allow"] = allow;
	}
	return result;
}

ResponsePlan	ErrorBuilder::buildDefault(http::e_status_code status)
{
	std::string			reason = http::Data::getStatusType(status);
	std::ostringstream	stream;
	stream	<< "<!DOCTYPE html>"
			<< "<html>\r\n"
			<< "<head><title>" << status << " " << reason << "</title></head>\r\n"
			<< "<body>\r\n"
			<< "<h1>" << status << " " << reason << "</h1>\r\n"
			<< "<p> An error has occured. </p>\r\n"
			<< "<a href=\"/\">Back to home</a>\r\n"
			<< "</body>\r\n"
			<< "</html>\r\n";
	std::string			body = stream.str();
	ResponsePlan		plan;

	plan.status = status;
	plan.addStandardHeaders();
	plan.headers["Content-Type"] = http::Data::getMimeType("html");
	plan.headers["Content-Length"] = str::toString(body.length());
	plan.body = new MemoryBodySource(body);

	return plan;
}

ResponsePlan	ErrorBuilder::buildFromErrorPage(
	http::e_status_code status,
	const std::string& path,
	const std::string &optionalDir
)
{
	std::string	target;
	if (fs::isExist(path))
		target = path;
	else
		target = optionalDir + path;
	int		fd = fs::openReadOnly(target);
	ssize_t	size = fs::size(target);

	if (fd < 0 || size < 0)
		return buildDefault(status);

	ResponsePlan plan;
	plan.status = status;
	plan.addStandardHeaders();
	plan.headers["Content-Type"] = http::Data::getMimeType("html");
	plan.headers["Content-Length"] = str::toString(size);
	plan.body = new FileBodySource(fd);

	return plan;
}