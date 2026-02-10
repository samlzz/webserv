#include "http/handlers/DeleteHandler.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"
#include "utils/fileSystemUtils.hpp"

#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>

ResponsePlan	DeleteHandler::handle(
								const HttpRequest &req,
								const routing::Context &route) const
{
	(void)req;
	std::string		path = route.location->root + route.normalizedPath;
	struct stat		st;

	if (!(fs::isExist(path, &st)))
		return ErrorBuilder::build(http::SC_NOT_FOUND, route.location);
	if (!fs::isFile(st))
		return ErrorBuilder::build(http::SC_FORBIDDEN, route.location);
	
	if (unlink(path.c_str()) != 0)
		return ErrorBuilder::build(
			(errno == EACCES || errno == EPERM)
				? http::SC_FORBIDDEN
				: http::SC_INTERNAL_SERVER_ERROR,
			route.location
		);
	
	ResponsePlan	plan;
	plan.status = http::SC_NO_CONTENT;
	plan.headers["Content-Length"] = "0";
	return (plan);
}