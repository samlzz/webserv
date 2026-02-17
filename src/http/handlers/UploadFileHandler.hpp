#ifndef __UPLOADFILEHANDLER_HPP__
# define __UPLOADFILEHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"
#include "http/response/ResponsePlan.hpp"

class UploadFileHandler : public IHttpHandler
{
private:
	ResponsePlan	handleOctetStream(
						const HttpRequest &req,
						const routing::Context &route) const;

	ResponsePlan	handleMultipart(
						const HttpRequest &req,
						const routing::Context &route) const;

	ResponsePlan	handleTextPlain(
						const HttpRequest &req,
						const routing::Context &route) const;
public:
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route) const;

};

#endif /* UPLOADFILEHANDLER_HPP */