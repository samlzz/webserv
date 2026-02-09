#ifndef __THEMEHANDLER_HPP__
#define __THEMEHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"

class ThemeHandler : public IHttpHandler
{
public:
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route) const;
};

#endif /* THEMEHANDLER_HPP */