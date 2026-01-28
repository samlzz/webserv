#ifndef __STATICFILEHANDLER_HPP__
# define __STATICFILEHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"

class StaticFileHandler : public IHttpHandler
{
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route) const;

};

#endif /* STATICFILEHANDLER_HPP */