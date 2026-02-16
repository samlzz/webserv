#include "http/handlers/UploadFileHandler.hpp"
#include "config/Config.hpp"
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
#include <vector>
#include <algorithm>

static std::string 	_removeTrailingSlashes(const std::string &path)
{
	std::string result;
	
	for (size_t i = 0; i < path.length(); ++i)
	{
		if (path[i] != '/' || (i == 0 || path[i - 1] != '/'))
		{
			result += path[i];
		}
	}

	return (result);
}

static inline std::string 	_generateFilename(const std::string &filename, http::e_body_kind contentType)
{
	std::string new_filename("");
	std::string	ext("");

	switch(contentType)
	{
		case http::CT_MULTIPART_FORM_DATA:
		{
			size_t ext_pos = filename.find_last_of(".");
			
			if (ext_pos != std::string::npos)
			{
				new_filename = filename.substr(0, ext_pos);
				ext = filename.substr(ext_pos);
			}
			else
				new_filename = filename;
			new_filename += "_" + str::toString(std::time(0)) + ext;
			break;
		}
		
		case http::CT_TEXT_PLAIN:
			new_filename = "text_post_" + str::toString(std::time(0)) + ".txt";
			break;

		case http::CT_BINARY:
			new_filename = "raw_post_" + str::toString(std::time(0));
			break;

		case http::CT_APPLICATION_X_WWW_FORM_URLENCODED: break;
		case http::CT_UNKNOWN: break;
		default: break;
	}
	return (new_filename);
}

static std::string 	_generateFilePath(
						const std::string &filename,
						http::e_body_kind contentType,
						http::e_method method,
						const Config::Server::Location *loc,
						ResponsePlan &plan)
{
	std::string uploadDir;
	std::string new_filename;

	if (method == http::MTH_PUT)
	{
		uploadDir = loc->root;
		new_filename = filename;
	}
	else
	{
		if (loc->uploadPath.isNone())
		{
			plan = ErrorBuilder::build(http::SC_FORBIDDEN, loc);
			return ("");
		}
		uploadDir = *loc->uploadPath;
		new_filename = _generateFilename(filename, contentType);
	}

	if (!uploadDir.empty() && uploadDir[uploadDir.length() - 1] == '/')
		uploadDir = uploadDir.substr(0, uploadDir.length() - 1);
	if (!new_filename.empty() && new_filename[0] == '/')
		new_filename = new_filename.substr(1);

	if (!fs::isDir(uploadDir) || access(uploadDir.c_str(), X_OK | W_OK) != 0)
	{
		plan = ErrorBuilder::build(http::SC_FORBIDDEN, loc);
		return ("");
	}
	if (new_filename.empty())
	{
		plan = ErrorBuilder::build(http::SC_BAD_REQUEST, loc);
		return ("");
	}

	return (uploadDir + "/" + new_filename);
}

static bool	writeFile(
				const std::string &path,
				const t_bytes &data,
				http::e_method method,
				ResponsePlan &plan,
				const Config::Server::Location *loc)
{
	int fd = -1;
	if (method == http::MTH_PUT)
		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (method == http::MTH_POST)
		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644);

	if (fd < 0)
	{
		if (errno == EACCES)
			plan = ErrorBuilder::build(http::SC_FORBIDDEN, loc);
		else if (errno == EEXIST)
			plan = ErrorBuilder::build(http::SC_CONFLICT, loc);
		else 
			plan = ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, loc);
		return (false);
	}

	size_t	totalWritten = 0;
	ssize_t	written;
	while (totalWritten < data.size())
	{
		written = write(fd, data.data() + totalWritten, data.size() - totalWritten);
		if (written < 0)
		{
			close(fd);
			plan = ErrorBuilder::build(http::SC_INTERNAL_SERVER_ERROR, loc);
			return (false);
		}
		totalWritten += static_cast<size_t>(written);
	}
	close(fd);
	return (written >= 0);
}

ResponsePlan	UploadFileHandler::handleTextPlain(
								const HttpRequest &req,
								const routing::Context &route) const
{
	ResponsePlan	plan;
	std::string filename = route.normalizedPath;
	std::string fullPath = _generateFilePath(filename,
										http::CT_TEXT_PLAIN,
										req.getMethod(),
										route.location, plan);
	if (fullPath.empty())
		return (plan);

	if (!writeFile(fullPath, req.getBody(), req.getMethod(), plan, route.location))
		return (plan);

	plan.status = http::SC_CREATED;
	plan.headers["Location"] = _removeTrailingSlashes(filename);
	plan.headers["Content-Length"] = "0";

	return (plan);
}

ResponsePlan	UploadFileHandler::handleOctetStream(
								const HttpRequest &req,
								const routing::Context &route) const
{
	ResponsePlan	plan;

	std::string filename = route.normalizedPath;
	std::string fullPath = _generateFilePath(filename,
										http::CT_BINARY,
										req.getMethod(),
										route.location, plan);
	if (fullPath.empty())
		return (plan);

	if (!writeFile(fullPath, req.getBody(), req.getMethod(), plan, route.location))
		return (plan);

	plan.status = http::SC_CREATED;
	plan.headers["Location"] = _removeTrailingSlashes(filename);
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

	std::vector<char>::const_iterator pBoundary_start;
	std::vector<char>::const_iterator pBoundary_end;

	const t_bytes		&vecBody = req.getBody();
	std::vector<char>::const_iterator vec_it = vecBody.begin();
	std::vector<char>::const_iterator vec_ite = vecBody.end();

	while (vec_it != vec_ite)
	{	
		pBoundary_start = std::search(vec_it, vec_ite, delimiter.begin(), delimiter.end());
		if (pBoundary_start == vec_ite)
			return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

		vec_it = pBoundary_start + delimiter.length();

		pBoundary_end = std::search(vec_it, vec_ite, delimiter.begin(), delimiter.end());
		if (pBoundary_end == vec_ite)
			break;

		// Extract filename and content
		std::vector<char> part_vec(vec_it, pBoundary_end);

		std::string toFind = "filename=\"";
		std::vector<char>::iterator pFilename_start = std::search(part_vec.begin(), part_vec.end(), toFind.begin(), toFind.end());
		if (pFilename_start != part_vec.end())
		{
			if (std::distance(pFilename_start, part_vec.end()) < 10)
				return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

			std::string toFind_end = "\"";
			std::vector<char>::iterator searchStart = pFilename_start + 10;
			std::vector<char>::iterator pFilename_end = std::search(searchStart, part_vec.end(), toFind_end.begin(), toFind_end.end());
			if (pFilename_end != part_vec.end())
			{
				// Extract filename
				std::string filename(pFilename_start + 10, pFilename_end);

				// Clean filename (remove path if any)
				size_t lastSlash = filename.find_last_of("/\\");
				if (lastSlash != std::string::npos)
					filename = filename.substr(lastSlash + 1);
				if (filename.empty())
					return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

				// Extract content
				std::string toFind_content = "\r\n\r\n";
				std::vector<char>::iterator pContent_start = std::search(part_vec.begin(), part_vec.end(), toFind_content.begin(), toFind_content.end());
				if (pContent_start != part_vec.end())
				{
					// Extract content and save to file
					std::vector<char>::iterator pContent_start_plus = pContent_start + toFind_content.length();
					std::vector<char> fileContent(pContent_start_plus, part_vec.end());

					//remove trailing \r\n
					if (fileContent.size() >= 2)
						fileContent.resize(fileContent.size() - 2);

					std::string fullPath = _generateFilePath(filename,
														http::CT_MULTIPART_FORM_DATA,
														req.getMethod(),
														route.location, plan);
					if (fullPath.empty())
						return (plan);

					if (!writeFile(fullPath, fileContent, req.getMethod(), plan, route.location))
						return (plan);
					plan.headers["Location"] = _removeTrailingSlashes(filename);
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
		vec_it = pBoundary_end;
	}

	plan.status = http::SC_CREATED;

	return (plan);
}

ResponsePlan	UploadFileHandler::handle(
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
			return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

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