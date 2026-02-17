#include "http/handlers/RedirectionHandler.hpp"
#include "http/HttpTypes.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"

ResponsePlan	RedirectionHandler::handle(
								const HttpRequest &req,
								const routing::Context &route) const
{
	(void)req;
	(void)route;

	ResponsePlan	plan;

	if (route.location->redirect.get()->code != http::SC_NONE)
		plan.status = route.location->redirect.get()->code;
	else
		plan.status = http::SC_MOVED_PERMANENTLY;
	plan.headers["Location"] = route.location->redirect.get()->path;
	plan.headers["Content-Length"] = "0";

	return (plan);
}