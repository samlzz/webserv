#ifndef __STATICFILEHANDLER_HPP__
# define __STATICFILEHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"

#include <string>

class StaticFileHandler : public IHttpHandler
{
private:
	ResponsePlan	loadAutoindex(
						const std::string &path,
						const routing::Context &route);
	
	ResponsePlan	loadFile(
						const std::string &path,
						const routing::Context &route);
public:
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route);

};

#endif /* STATICFILEHANDLER_HPP */