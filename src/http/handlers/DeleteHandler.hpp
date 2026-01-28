#ifndef __DELETEHANDLER_HPP__
# define __DELETEHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"

class DeleteHandler : public IHttpHandler
{
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route) const;

};

#endif /* DELETEHANDLER_HPP */