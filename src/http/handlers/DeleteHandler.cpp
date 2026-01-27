#include "http/handlers/DeleteHandler.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"

#include <sys/stat.h>
#include <unistd.h>

ResponsePlan	DeleteHandler::handle(
								const HttpRequest &req,
								const routing::Context &route)
{
	(void)req;
	(void)route;

	ResponsePlan	plan;

	std::string path = route.location->root + req.getPath();
	struct stat st;

	if (path[0] == '/')
		path = "." + path;

	if (stat(path.c_str(), &st) != 0)
		plan.status = http::SC_NOT_FOUND;
	else if (S_ISDIR(st.st_mode))
		plan.status = http::SC_FORBIDDEN;
	else if (unlink(path.c_str()) != 0)
		plan.status = http::SC_INTERNAL_SERVER_ERROR;
	else
		plan.status = http::SC_NO_CONTENT;

	plan.headers["Content-Length"] = "0";
	
	return (plan);
}