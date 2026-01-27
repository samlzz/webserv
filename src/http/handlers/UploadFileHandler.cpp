#include "http/handlers/UploadFileHandler.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"

ResponsePlan	UploadFileHandler::handle(
								const HttpRequest &req,
								const routing::Context &route)
{
	(void)req;
	(void)route;

	ResponsePlan	plan;


	return (plan);
}