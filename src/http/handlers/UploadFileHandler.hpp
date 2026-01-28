#ifndef __UPLOADFILEHANDLER_HPP__
# define __UPLOADFILEHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"

class UploadFileHandler : public IHttpHandler
{
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route) const;

};

#endif /* UPLOADFILEHANDLER_HPP */