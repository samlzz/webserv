#ifndef __UPLOADFILEHANDLER_HPP__
# define __UPLOADFILEHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"
#include "http/response/ResponsePlan.hpp"

#include <vector>

class UploadFileHandler : public IHttpHandler
{
private:
	ResponsePlan	handleOctetStream(
						const HttpRequest &req,
						const routing::Context &route) const;

	ResponsePlan	handleMultipart(
						const HttpRequest &req,
						const routing::Context &route,
						std::vector<std::string> &uploaded_files) const;

	ResponsePlan	handleTextPlain(
						const HttpRequest &req,
						const routing::Context &route) const;
public:
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route) const;

};

#endif /* UPLOADFILEHANDLER_HPP */