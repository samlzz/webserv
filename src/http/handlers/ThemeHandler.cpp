
#include "http/handlers/IHttpHandler.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/handlers/ThemeHandler.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"

ResponsePlan	ThemeHandler::handle(
						const HttpRequest &req,
						const routing::Context &route) const
{
	(void)req;
	(void)route;

	ResponsePlan	plan;

	std::string mode = "light";
	if (req.getQuery().find("mode=dark") != std::string::npos)
		mode = "dark";

	route.cookies.setCookie("theme", mode);

	plan.status = http::SC_FOUND;

	plan.headers["Location"] = "/";
	plan.headers["Content-Length"] = "0";

	return plan;
}