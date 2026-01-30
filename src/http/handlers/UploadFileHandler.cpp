#include "http/handlers/UploadFileHandler.hpp"
#include "http/HttpTypes.hpp"
#include "http/response/BuffStream.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/HttpData.hpp"
#include "utils/stringUtils.hpp"
#include "utils/fileSystemUtils.hpp"
#include "bodySrcs/MemoryBodySource.hpp"

#include <cerrno>
#include <cstddef>
#include <ctime>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

std::string 	UploadFileHandler::generateFilename(
						const std::string &filename,
						http::e_body_kind contentType) const
{
	std::string new_filename = "";

	switch(contentType)
	{
		case http::CT_MULTIPART_FORM_DATA:
		{
			size_t ext_pos = filename.find_last_of(".");
			
			if (ext_pos != std::string::npos)
				new_filename = filename.substr(0, ext_pos) + "_" + str::toString(std::time(0)) + filename.substr(ext_pos);
			else
				new_filename = filename + "_" + str::toString(std::time(0));
			break;
		}
		
		case http::CT_TEXT_PLAIN:
			new_filename = "text_post_" + str::toString(std::time(0)) + ".txt";
			break;

		case http::CT_BINARY:
			new_filename = "raw_post_" + str::toString(std::time(0));
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
						http::e_method method,
						ResponsePlan &plan) const
{
	std::string uploadDir;
	std::string new_filename = filename;
	if (method == http::MTH_POST)
	{
		uploadDir = *route.location->uploadPath.get();
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

	//check uploadDir
	if (!fs::isDir(uploadDir))
	{
		plan = ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, route.location);
		return ("");
	}
	if (!(fs::checkPerms(uploadDir, fs::P_WRITE | fs::P_EXEC)))
	{
		plan = ErrorBuilder::build(http::SC_FORBIDDEN, route.location);
		return ("");
	}
	if (new_filename.empty())
	{
		plan = ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
		return ("");
	}

	return (uploadDir + "/" + new_filename);
}

bool			UploadFileHandler::writeFile(
						const std::string &path,
						const t_bytes &data,
						http::e_method method,
						ResponsePlan &plan,
						const routing::Context &route) const
{
	int fd = -1;
	if (method == http::MTH_PUT)
		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (method == http::MTH_POST)
		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644);

	if (fd < 0)
	{
		if (errno == EACCES)
			plan = ErrorBuilder::build(http::SC_FORBIDDEN, route.location);
		else if (errno == EEXIST)
			plan = ErrorBuilder::build(http::SC_CONFLICT, route.location);
		else 
			plan = ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, route.location);
		return (false);
	}

	size_t totalWritten = 0;
	ssize_t written;
	while (totalWritten < data.size())
	{
		written = write(fd, data.data() + totalWritten, data.size() - totalWritten);
		if (written < 0)
		{
			close(fd);
			plan = ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, route.location);
			return (false);
		}
		totalWritten += static_cast<size_t>(written);
	}
	close(fd);
	if (written < 0)
		return (false);
	return (true);
}

ResponsePlan	UploadFileHandler::handleTextPlain(
								const HttpRequest &req,
								const routing::Context &route) const
{
	ResponsePlan	plan;
	std::string filename = route.normalizedPath;
	std::string fullPath = generateFilePath(route, filename,
			http::CT_TEXT_PLAIN, req.getMethod(), plan);
	if (fullPath.empty())
		return (plan);

	// std::string body(req.vec.data(), req.vec.size());
	
	if (fullPath[0] == '\0')
		fullPath = '.' + fullPath;

	if (!writeFile(fullPath, req.getBody(), req.getMethod(), plan, route))
		return (plan);

	//code 201 Created
	plan.status = http::SC_CREATED;

	plan.headers["Location"] = "/" + filename;
	plan.headers["Content-Length"] = "0";

	return (plan);
}

ResponsePlan	UploadFileHandler::handleOctetStream(
								const HttpRequest &req,
								const routing::Context &route) const
{
	ResponsePlan	plan;

	std::string filename = route.normalizedPath;
	std::string fullPath = generateFilePath(route, filename,
			http::CT_BINARY, req.getMethod(), plan);
	if (fullPath.empty())
		return (plan);

	// std::string body(req.getBody().data(), req.getBody().size());

	if (!writeFile(fullPath, req.getBody(), req.getMethod(), plan, route))
		return (plan);

	//code 201 Created
	plan.status = http::SC_CREATED;

	plan.headers["Location"] = "/" + filename;
	plan.headers["Content-Length"] = "0";

	return (plan);
}

ResponsePlan	UploadFileHandler::handleMultipart(
								const HttpRequest &req,
								const routing::Context &route) const
{
	ResponsePlan	plan;

	std::string contentType = req.getField("Content-Type");
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
	t_bytes		vecBody = req.getBody();
	std::string	body(vecBody.data(), vecBody.size());
	
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
				{
					return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
				}
				// Extract content
				size_t	contentPos = part.find("\r\n\r\n");
				if (contentPos != std::string::npos)
				{
					size_t headerSize = contentPos + 4;
					size_t endSize = 2;
					if (part.length() <= headerSize + endSize)
					{
						return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
					}
					
					// Extract content and save to file
					std::string	fileContent = part.substr(contentPos + 4, part.length() - contentPos);

					std::string fullPath = generateFilePath(route, filename,
							http::CT_MULTIPART_FORM_DATA, req.getMethod(), plan);
					if (fullPath.empty())
						return (plan);

					if (!writeFile(fullPath, t_bytes(fileContent.begin(), fileContent.end()), req.getMethod(), plan, route))
						return (plan);

					plan.headers["Location"] = "/" + filename;
				}
				else
				{
					return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
				}
			}
			else
			{
				return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
			}
		}
		else
		{
			return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);
		}

		pos = nextPos;
	}

	plan.status = http::SC_CREATED;

	return (plan);
}

ResponsePlan	UploadFileHandler::handleContentType(
								const HttpRequest &req,
								const routing::Context &route) const
{
	ResponsePlan	plan;

	std::string contentType = req.getField("Content-Type");
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

	if (plan.status == http::SC_CREATED)
	{
		std::string body_response = "<html><body><h1>File(s) uploaded successfully</h1><a title=\"GO BACK\" href=\"/\">go back</a></body></html>\n";
		
		plan.headers["Content-Type"] = http::Data::getMimeType("html");
		plan.headers["Content-Length"] = str::toString(body_response.size());

		plan.body = new MemoryBodySource(body_response);
	}
	return (plan);
}

ResponsePlan	UploadFileHandler::handle(
								const HttpRequest &req,
								const routing::Context &route) const
{
	if (req.getMethod() == http::MTH_PUT || req.getMethod() == http::MTH_POST)
	{
		return (handleContentType(req, route));
	}
	else
		return (ErrorBuilder::build(http::SC_METHOD_NOT_ALLOWED,
				route.location));
}