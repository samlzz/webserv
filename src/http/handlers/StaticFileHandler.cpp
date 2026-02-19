#include "http/handlers/StaticFileHandler.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"
#include "http/HttpData.hpp"
#include "bodySrcs/MemoryBodySource.hpp"
#include "bodySrcs/FileBodySource.hpp"
#include "utils/pathUtils.hpp"
#include "utils/stringUtils.hpp"
#include "utils/fileSystemUtils.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <vector>

ResponsePlan	StaticFileHandler::loadAutoindex(
										const std::string &path,
										http::e_method method,
										const routing::Context &route) const
{
	DIR *dir = opendir(path.c_str());
	if (!dir)
		return (ErrorBuilder::build(http::SC_FORBIDDEN, route.location));

	struct dirent				*entry;
	std::vector<std::string>	folders;
	std::vector<std::string>	files;

	while ((entry = readdir(dir)) != NULL)
	{
		if (*entry->d_name == '.')
			continue;
		struct stat st;
		if (fs::isExist(path + "/" + entry->d_name, &st))
		{
			if (fs::isDir(st))
				folders.push_back(std::string(entry->d_name) + '/');
			if (fs::isFile(st))
				files.push_back(entry->d_name);
		}
	}

	std::string body;
	body = "<html><head><title>Index of " + route.normalizedPath + "</title></head><body>";
	body += "<h1>Index of " + route.normalizedPath + "</h1><hr><pre>";

	std::sort(folders.begin(), folders.end());
	std::sort(files.begin(), files.end());

	for (size_t i = 0; i < folders.size(); ++i)
		body += "<a href=\"" + folders[i] + "\">" + folders[i] + "</a><br>\n";
	for (size_t i = 0; i < files.size(); ++i)
		body += "<a href=\"" + files[i] + "\">" + files[i] + "</a><br>\n";
	body += "</pre><hr></body></html>";

	closedir(dir);

	ResponsePlan	plan;
	plan.status = http::SC_OK;
	plan.headers["Content-Type"] = http::Data::getMimeType("html");
	plan.headers["Content-Length"] = str::toString(body.size());
	if (method != http::MTH_HEAD)
		plan.body = new MemoryBodySource(body);
	
	return (plan);
}

ResponsePlan	StaticFileHandler::loadFile(
									const std::string &path,
									size_t fileSize,
									http::e_method method,
									const routing::Context &route
) const
{
	int fd = fs::openReadOnly(path);
	if (fd < 0)
		return (ErrorBuilder::build(http::SC_FORBIDDEN, route.location));

	ResponsePlan	plan;

	plan.status = http::SC_OK;
	plan.headers["Content-Length"] = str::toString(fileSize);
	plan.headers["Content-Type"] = http::Data::getMimeType(path::subExt(path));

	if (method == http::MTH_HEAD)
		close(fd);
	else
		plan.body = new FileBodySource(fd);

	return (plan);
}

ResponsePlan	StaticFileHandler::handle(
								const HttpRequest &req,
								const routing::Context &route) const
{
	struct stat st;
	std::string path = route.location->root + route.normalizedPath;

	if (!fs::isExist(path, &st))
		return ErrorBuilder::build(http::SC_NOT_FOUND, route.location);

	if (fs::isFile(st))
		return loadFile(path, st.st_size, req.getMethod(), route);

	if (fs::isDir(st))
	{
		if (path[path.length() - 1] != '/')
		{
			ResponsePlan	plan;
			plan.headers["Location"] = req.getPath() + "/";
			plan.headers["Content-Length"] = "0";
			plan.status = http::SC_MOVED_PERMANENTLY;
			return (plan);
		}

		std::string fullIndex = path + route.location->index;
		if (fs::isExist(fullIndex, &st) && fs::isFile(st))
			return loadFile(fullIndex, st.st_size, req.getMethod(), route);

		if (route.location->autoindex)
			return loadAutoindex(path, req.getMethod(), route);
	}

	return ErrorBuilder::build(http::SC_NOT_FOUND, route.location);
}