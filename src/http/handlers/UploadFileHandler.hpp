#ifndef __UPLOADFILEHANDLER_HPP__
# define __UPLOADFILEHANDLER_HPP__

#include "http/handlers/IHttpHandler.hpp"
#include "http/response/ResponsePlan.hpp"

class UploadFileHandler : public IHttpHandler
{
private:
	bool			writeFile(
						const std::string &path,
						const std::string &data,
						http::e_method method);

	std::string 	generateFilePath(
						const routing::Context &route,
						const std::string &filename,
						http::e_body_kind contentType,
						http::e_method method);

	std::string 	generateFilename(
						const std::string &filename,
						http::e_body_kind contentType);

	ResponsePlan	handleContentType(
						const HttpRequest &req,
						const routing::Context &route);	
						
	ResponsePlan	handleOctetStream(
						const HttpRequest &req,
						const routing::Context &route);

	ResponsePlan	handleMultipart(
						const HttpRequest &req,
						const routing::Context &route);

	ResponsePlan	handleTextPlain(
						const HttpRequest &req,
						const routing::Context &route);
public:
	ResponsePlan	handle(
						const HttpRequest &req,
						const routing::Context &route);

};

#endif /* UPLOADFILEHANDLER_HPP */