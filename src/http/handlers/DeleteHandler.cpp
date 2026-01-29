#include "http/handlers/DeleteHandler.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/fileSystemUtils.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"

#include <sys/stat.h>
#include <unistd.h>

ResponsePlan	DeleteHandler::handle(
								const HttpRequest &req,
								const routing::Context &route) const
{
	(void)req;

	ResponsePlan	plan;

	std::string	path = route.location->root + route.normalizedPath;

	if (path[0] == '/')
		path = "." + path;

	if (fs::checkPerms(path, fs::P_EXIST))
		return ErrorBuilder::build(http::SC_NOT_FOUND, route.location);
	else if (fs::isDir(path))
		return ErrorBuilder::build(http::SC_FORBIDDEN, route.location);
	else if (unlink(path.c_str()) != 0)
		return ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, route.location);
	else
		plan.status = http::SC_NO_CONTENT;

	plan.headers["Content-Length"] = "0";
	return (plan);
}