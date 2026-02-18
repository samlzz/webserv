/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorBuilder.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:49:38 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/18 13:11:07 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>

#include "config/Config.hpp"
#include "ErrorBuilder.hpp"
#include "ft_log/LogOp.hpp"
#include "ft_log/level.hpp"
#include "http/HttpData.hpp"
#include "http/handlers/bodySrcs/FileBodySource.hpp"
#include "http/handlers/bodySrcs/MemoryBodySource.hpp"
#include "http/response/ResponsePlan.hpp"
#include "log.h"
#include "utils/fileSystemUtils.hpp"
#include "utils/stringUtils.hpp"

ResponsePlan	ErrorBuilder::build(
	http::e_status_code status,
	const Config::Server::Location *location
)
{
	ft_log::e_log_level	lvl = status >= http::SC_INTERNAL_SERVER_ERROR
								? ft_log::LOG_ERROR : ft_log::LOG_WARN;
	ft_log::log(WS_LOG_HTTP, lvl) << "HTTP " << status
		<< " error response built" << std::endl;

	if (location)
	{
		std::string	errPage;

		Config::t_errPages::const_iterator it = location->errorPages.find(status);
		if (it != location->errorPages.end())
			errPage = it->second;

		if (!location->defaultErrPage.empty())
			errPage = location->defaultErrPage;

		if (!errPage.empty())
		{
			ft_log::log(WS_LOG_HTTP, ft_log::LOG_DEBUG)
				<< "Error page source: " << errPage << std::endl;
			return buildFromErrorPage(status, errPage);
		}
	}
	ft_log::log(WS_LOG_HTTP, ft_log::LOG_DEBUG)
		<< "Using default error page" << std::endl;
	return buildDefault(status);
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
	plan.headers["Content-Type"] = http::Data::getMimeType("html");
	plan.headers["Content-Length"] = str::toString(body.length());
	plan.body = new MemoryBodySource(body);

	return plan;
}

ResponsePlan	ErrorBuilder::buildFromErrorPage(
	http::e_status_code status,
	const std::string& path
)
{
	int		fd = fs::openReadOnly(path);
	ssize_t	size = fs::size(path);

	if (fd < 0 || size < 0)
		return buildDefault(status);

	ResponsePlan plan;
	plan.status = status;
	plan.headers["Content-Type"] = http::Data::getMimeType("html");
	plan.headers["Content-Length"] = str::toString(size);
	plan.body = new FileBodySource(fd);

	return plan;
}