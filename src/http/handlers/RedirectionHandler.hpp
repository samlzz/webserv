#ifndef __REDIRECTIONHANDLER_HPP__
# define __REDIRECTIONHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"

class RedirectionHandler : public IHttpHandler
{
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route);

};

#endif /* REDIRECTIONHANDLER_HPP */