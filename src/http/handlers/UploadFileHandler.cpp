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
	std::string filename = route.normalizedUri;
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

	std::string filename = route.normalizedUri;
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

static bool	_extractBoundary(const std::string &contentType, std::string &delimiter)
{
	size_t	boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos)
		return (false);
	delimiter = "--" + contentType.substr(boundaryPos + 9);
	return (true);
}

static bool	_extractFilename(const std::vector<char> &part, std::string &filename)
{
	std::string toFind = "filename=\"";
	std::vector<char>::const_iterator it = std::search(
		part.begin(), part.end(), toFind.begin(), toFind.end());
	if (it == part.end())
		return (false);
	if (std::distance(it, part.end()) < 10)
		return (false);

	std::vector<char>::const_iterator searchStart = it + 10;
	std::string quote = "\"";
	std::vector<char>::const_iterator endIt = std::search(
		searchStart, part.end(), quote.begin(), quote.end());
	if (endIt == part.end())
		return (false);

	filename.assign(it + 10, endIt);

	// Clean filename (remove path if any)
	size_t lastSlash = filename.find_last_of("/\\");
	if (lastSlash != std::string::npos)
		filename = filename.substr(lastSlash + 1);
	return (!filename.empty());
}

static bool	_extractContent(const std::vector<char> &part, t_bytes &content)
{
	std::string sep = "\r\n\r\n";
	std::vector<char>::const_iterator it = std::search(
		part.begin(), part.end(), sep.begin(), sep.end());
	if (it == part.end())
		return (false);

	content.assign(it + sep.length(), part.end());
	// remove trailing \r\n
	if (content.size() >= 2)
		content.resize(content.size() - 2);
	return (true);
}

ResponsePlan	UploadFileHandler::handleMultipart(
								const HttpRequest &req,
								const routing::Context &route,
								std::vector<std::string> &uploaded_files) const
{
	ResponsePlan	plan;

	std::string contentType = req.getField("Content-Type");
	if (contentType.empty())
		return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

	// Extract boundary delimiter
	std::string delimiter;
	if (!_extractBoundary(contentType, delimiter))
		return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

	const t_bytes	&vecBody = req.getBody();
	t_bytes::const_iterator vec_it = vecBody.begin();
	t_bytes::const_iterator vec_ite = vecBody.end();

	while (vec_it != vec_ite)
	{
		//Extract boundary part
		t_bytes::const_iterator partStart = std::search(
			vec_it, vec_ite, delimiter.begin(), delimiter.end());
		if (partStart == vec_ite)
			return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

		vec_it = partStart + delimiter.length();

		t_bytes::const_iterator partEnd = std::search(
			vec_it, vec_ite, delimiter.begin(), delimiter.end());
		if (partEnd == vec_ite)
			break;
		//Extract end of boundary part

		t_bytes part(vec_it, partEnd);

		//Extract filename
		std::string filename;
		if (!_extractFilename(part, filename))
			return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

		//Extract content
		t_bytes fileContent;
		if (!_extractContent(part, fileContent))
			return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

		// Write file
		std::string fullPath = _generateFilePath(filename,
			http::CT_MULTIPART_FORM_DATA, req.getMethod(), route.location, plan);
		if (fullPath.empty())
			return (plan);

		if (!writeFile(fullPath, fileContent, req.getMethod(), plan, route.location))
			return (plan);

		uploaded_files.push_back(fullPath);
		plan.headers["Location"] = _removeTrailingSlashes(filename);

		//Go to the next boundary if exist
		vec_it = partEnd;
	}

	plan.status = http::SC_CREATED;
	return (plan);
}

ResponsePlan	UploadFileHandler::handle(
								const HttpRequest &req,
								const routing::Context &route) const
{
	ResponsePlan	plan;

	std::vector<std::string> uploaded_files;
	std::string contentType = req.getField("Content-Type");
	if (contentType.empty())
		contentType = "application/octet-stream";
	
	switch(http::Data::getContentTypeKind(contentType))
	{
		case http::CT_APPLICATION_X_WWW_FORM_URLENCODED:
			return ErrorBuilder::build(http::SC_BAD_REQUEST, route.location);

		case http::CT_MULTIPART_FORM_DATA:
			plan = handleMultipart(req, route, uploaded_files);
			if (plan.status != http::SC_CREATED)
			{
				for (std::vector<std::string>::iterator it = uploaded_files.begin(); it != uploaded_files.end(); ++it)
				{
					if (unlink(it->c_str()) != 0)
						return ErrorBuilder::build((errno == EACCES || errno == EPERM)? http::SC_FORBIDDEN: http::SC_INTERNAL_SERVER_ERROR,route.location);
				}
			}
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