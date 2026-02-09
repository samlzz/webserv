#ifndef __FORMHANDLER_HPP__
# define __FORMHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"
#include "http/request/HttpRequest.hpp"
#include <string>

class FormHandler : public IHttpHandler
{
private:
	// typedef  void (ActionHandler*) (const HttpRequest &req, const routing::Context &route) const;

	// std::map<std::string, ActionHandler*>	_actionHandlers;
	// void setUserRole(const HttpRequest &req, const routing::Context &route) const;

public:
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route) const;

};

#endif /* FORMHANDLER_HPP */