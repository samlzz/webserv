#ifndef __FORMHANDLER_HPP__
# define __FORMHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"

class FormHandler : public IHttpHandler
{
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route);

};

#endif /* FORMHANDLER_HPP */