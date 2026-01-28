#include "http/handlers/UploadFileHandler.hpp"
#include "http/HttpTypes.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "config/validation/configValidate.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/HttpData.hpp"
#include "http/fileSystemUtils.hpp"

#include <cstddef>
#include <ctime>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

std::string 	UploadFileHandler::generateFilename(
						const std::string &filename,
						http::e_body_kind contentType)
{
	std::string new_filename = "";

	switch(contentType)
	{
		case http::CT_MULTIPART_FORM_DATA:
		{
			size_t ext_pos = filename.find_last_of(".");
			
			if (ext_pos != std::string::npos)
				new_filename = filename.substr(0, ext_pos) + "_" + toString(std::time(0)) + filename.substr(ext_pos);
			else
				new_filename = filename + "_" + toString(std::time(0));
			break;
		}
		
		case http::CT_TEXT_PLAIN:
			new_filename = "text_post_" + toString(std::time(0)) + ".txt";
			break;

		case http::CT_BINARY:
			new_filename = "raw_post_" + toString(std::time(0));
			break;

		case http::CT_APPLICATION_X_WWW_FORM_URLENCODED:
			break;
		
		case http::CT_UNKNOWN:
			break;

		default:
			break;
	}

	return (new_filename);
}

std::string 	UploadFileHandler::generateFilePath(
						const routing::Context &route,
						const std::string &filename,
						http::e_body_kind contentType,
						http::e_method method)
{
	std::string uploadDir;
	std::string new_filename = filename;
	if (method == http::MTH_POST)
	{
		uploadDir = *route.location->uploadPath;
		if (uploadDir.empty())
			return ("");
		if (uploadDir[uploadDir.length() - 1] == '/')
			uploadDir = uploadDir.substr(0, uploadDir.length() - 1);

		switch(contentType)
		{
			case http::CT_MULTIPART_FORM_DATA:
				new_filename = generateFilename(filename, http::CT_MULTIPART_FORM_DATA);
				break;

			case http::CT_TEXT_PLAIN:
				new_filename = generateFilename(filename, http::CT_TEXT_PLAIN);
				break;

			case http::CT_BINARY:
				new_filename = generateFilename(filename, http::CT_BINARY);
				break;

			case http::CT_APPLICATION_X_WWW_FORM_URLENCODED:
				break;

			case http::CT_UNKNOWN:
				break;

			default:
				break;
		}
	}
	else if (method == http::MTH_PUT)
	{
		uploadDir = route.location->root;
		if (!new_filename.empty() && new_filename[0] == '/')
			new_filename = new_filename.substr(1);

		new_filename = filename;
	}
	return (uploadDir + "/" + new_filename);
}
bool			UploadFileHandler::writeFile(
						const std::string &path,
						const std::string &data,
						http::e_method method)
{
	int fd = -1;
	if (method == http::MTH_PUT)
		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (method == http::MTH_POST)
		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644);

	if (fd < 0)
		return (false);

	ssize_t totalWritten = 0;
	ssize_t written;
	while (totalWritten < static_cast<ssize_t>(data.length()))
	{
		written = write(fd, data.c_str() + totalWritten, data.length() - totalWritten);
		if (written < 0)
		{
			close(fd);
			return (false);
		}
		totalWritten += written;
	}
	close(fd);
	if (written < 0)
		return (false);
	return (true);
}
ResponsePlan	UploadFileHandler::handleTextPlain(
								const HttpRequest &req,
								const routing::Context &route)
{
	ResponsePlan	plan;

	std::string filename = route.normalizedPath;
	std::string fullPath = generateFilePath(route, filename,
			http::CT_TEXT_PLAIN, req.getMethod());

	// std::string body(req.vec.data(), req.vec.size());

	if (!writeFile(fullPath, req.getBody(), req.getMethod()))
		return ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, route.location);

	//code 201 Created
	plan.status = http::SC_CREATED;

	plan.headers["Location"] = "/" + filename;
	plan.headers["Content-Length"] = "0";

	return (plan);
}
ResponsePlan	UploadFileHandler::handleOctetStream(
								const HttpRequest &req,
								const routing::Context &route)
{
	ResponsePlan	plan;

	std::string filename = route.normalizedPath;
	std::string fullPath = generateFilePath(route, filename,
			http::CT_BINARY, req.getMethod());

	// std::string body(req.vec.data(), req.vec.size());

	if (!writeFile(fullPath, req.getBody(), req.getMethod()))
		return ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, route.location);

	//code 201 Created
	plan.status = http::SC_CREATED;

	plan.headers["Location"] = "/" + filename;
	plan.headers["Content-Length"] = "0";

	return (plan);
}
ResponsePlan	UploadFileHandler::handleMultipart(
								const HttpRequest &req,
								const routing::Context &route)
{
	ResponsePlan	plan;

	std::string contentType = req.getHeader("Content-Type");
	if (contentType.empty())
		return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

	// Extract boundary
	size_t	boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos)
		return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
	std::string	boundary = contentType.substr(boundaryPos + 9);
	std::string delimiter = "--" + boundary;

	size_t	pos = 0;

	// std::string body(req.vec.data(), req.vec.size());
	std::string	body = req.getBody();

	while ((pos = body.find(delimiter, pos)) != std::string::npos)
	{	
		// Find next delimiter
		size_t	nextPos = body.find(delimiter, pos + delimiter.length());
		if (nextPos == std::string::npos)
			break;

		// Extract filename and content
		std::string	part = body.substr(pos + delimiter.length(), nextPos - pos - delimiter.length());
		size_t	filenamePos = part.find("filename=\"");
		if (filenamePos != std::string::npos)
		{
			size_t	filenameEnd = part.find("\"", filenamePos + 10);
			if (filenameEnd != std::string::npos)
			{
				// Extract filename
				std::string	filename = part.substr(filenamePos + 10, filenameEnd - filenamePos - 10);

				// Clean filename (remove path if any)
				size_t lastSlash = filename.find_last_of("/\\");
				if (lastSlash != std::string::npos)
					filename = filename.substr(lastSlash + 1);
				if (filename.empty())
					return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

				// Extract content
				size_t	contentPos = part.find("\r\n\r\n");
				if (contentPos != std::string::npos)
				{
					size_t headerSize = contentPos + 4;
					size_t endSize = 2;
					if (part.length() >= headerSize + endSize)
						return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
					
					// Extract content and save to file
					std::string	fileContent = part.substr(contentPos + 4, part.length() - contentPos);

					std::string fullPath = generateFilePath(route, filename,
							http::CT_MULTIPART_FORM_DATA, req.getMethod());

					if (!writeFile(fullPath, fileContent, req.getMethod()))
						return ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, route.location);
					plan.headers["Location"] = "/" + filename;
				}
				else
					return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
			}
			else
				return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
		}
		else
			return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

		pos = nextPos;
	}

	//code 201 Created
	plan.status = http::SC_CREATED;

	plan.headers["Content-Length"] = "0";
	return (plan);

}

ResponsePlan	UploadFileHandler::handleContentType(
								const HttpRequest &req,
								const routing::Context &route)
{
	ResponsePlan	plan;

	std::string contentType = req.getHeader("Content-Type");
	if (contentType.empty())
		contentType = "application/octet-stream";
	
	switch(http::Data::getContentTypeKind(contentType))
	{
		case http::CT_APPLICATION_X_WWW_FORM_URLENCODED:
			//check return erreur bad request???
			break;

		case http::CT_MULTIPART_FORM_DATA:
			plan = handleMultipart(req, route);
			break;

		case http::CT_BINARY:
			plan = handleOctetStream(req, route);
			break;

		case http::CT_TEXT_PLAIN:
			plan = handleTextPlain(req, route);
			break;

		default:
			return ErrorBuilder::build(http::SC_UNSUPPORTED_MEDIA_TYPE,
					route.location);
	}

	return (plan);
}

ResponsePlan	UploadFileHandler::handle(
								const HttpRequest &req,
								const routing::Context &route)
{
	if (req.getMethod() == http::MTH_PUT || req.getMethod() == http::MTH_POST)
	{
		return (handleContentType(req, route));
	}
	else
		return (ErrorBuilder::build(http::SC_METHOD_NOT_ALLOWED,
				route.location));
}