#ifndef __STATICFILEHANDLER_HPP__
# define __STATICFILEHANDLER_HPP__

#include "http/HttpTypes.hpp"
#include "http/handlers/IHttpHandler.hpp"

#include <string>

class StaticFileHandler : public IHttpHandler
{
private:
	ResponsePlan	loadAutoindex(
						const std::string &path,
						http::e_method method,
						const routing::Context &route) const;
	
	ResponsePlan	loadFile(
						const std::string &path,
						size_t fileSize,
						http::e_method method,
						const routing::Context &route) const;
public:
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route) const;

};

#endif /* STATICFILEHANDLER_HPP */