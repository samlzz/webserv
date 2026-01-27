#include "http/handlers/RedirectionHandler.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"

ResponsePlan	RedirectionHandler::handle(
								const HttpRequest &req,
								const routing::Context &route)
{
	(void)req;
	(void)route;

	ResponsePlan	plan;

	if (route.location->redirect.get()->code != http::SC_NONE)
		plan.status = route.location->redirect.get()->code;
	else
		plan.status = http::SC_FOUND;
	plan.headers["Location"] = route.location->redirect.get()->path;

	return (plan);
}