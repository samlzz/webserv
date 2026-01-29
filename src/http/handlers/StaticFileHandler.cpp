#include "http/handlers/StaticFileHandler.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"
#include "http/HttpData.hpp"
#include "bodySrcs/MemoryBodySource.hpp"
#include "bodySrcs/FileBodySource.hpp"


#include "sys/stat.h"
#include "fcntl.h"
#include "utils/stringUtils.hpp"
#include <string>
#include <dirent.h>
#include <algorithm>

static inline std::string	subExt(const std::string& pPath)
{
	std::string		result;
	size_t			start;

	if ((start = pPath.find_last_of('.')) == std::string::npos)
		return ("");

	result = pPath.substr(start + 1, pPath.length() - start);
	if (result.empty())
		return ("");

	return (result);
}

ResponsePlan	StaticFileHandler::handle(
								const HttpRequest &req,
								const routing::Context &route) const
{
	(void)req;
	(void)route;
	// std::string		convertStr(vec.data(), vec.size());

	ResponsePlan	plan;

	struct stat st;
	std::string path = route.location->root + req.getPath();

	if (stat(path.c_str(), &st) != 0)
		plan.status = http::SC_NOT_FOUND;

	if (S_ISDIR(st.st_mode))
	{
		if (path[path.length() - 1] != '/')
		{
			std::string redirectPath = req.getPath() + "/";
			plan.headers["Location"] = redirectPath;
			plan.headers["Content-Length"] = "0";
			plan.status = http::SC_MOVED_PERMANENTLY;
			return (plan);
		}

		std::string fullIndex = path + route.location->index;
		if (stat(fullIndex.c_str(), &st) == 0 && S_ISREG(st.st_mode))
		{
			path = fullIndex;
			stat(fullIndex.c_str(), &st);
			std::string	ext = subExt(path);

			int _fd;
			if ((_fd = open(path.c_str(), O_RDONLY)) < 0)
			{
				plan.status = http::SC_FORBIDDEN;
				return (plan);
			}
			plan.headers["Content-Length"] = str::toString(st.st_size);
			plan.headers["Content-Type"] = http::Data::getMimeType(ext);
			
			plan.body = new FileBodySource(_fd);

			plan.status = http::SC_OK;
			return (plan);
		}

		if (route.location->autoindex)
		{			
			DIR *dir = opendir(path.c_str());
			if (!dir)
			{
				plan.status = http::SC_FORBIDDEN;
				return (plan);
			}
	
			struct dirent				*entry;
			std::vector<std::string>	folders;
			std::vector<std::string>	files;

			while ((entry = readdir(dir)) != NULL)
			{
				struct stat s;
				std::string fullEntryPath = path + "/" + entry->d_name;
				if (stat(fullEntryPath.c_str(), &s) == 0)
				{
					if (S_ISDIR(s.st_mode))
						folders.push_back(entry->d_name);
					if (S_ISREG(s.st_mode))
						files.push_back(entry->d_name);
				}
			}
			std::string body;
			body = "<html><head><title>Index of " + req.getPath() + "</title></head><body>";
			body += "<h1>Index of " + req.getPath() + "</h1><hr><pre>";

			std::sort(folders.begin(), folders.end());
			std::sort(files.begin(), files.end());

			for (size_t i = 0; i < folders.size(); ++i)
				body += "<a href=\"" + folders[i] + "\">" + folders[i] + "</a><br>\n";
			for (size_t i = 0; i < files.size(); ++i)
				body += "<a href=\"" + files[i] + "\">" + files[i] + "</a><br>\n";
			body += "</pre><hr></body></html>";

			plan.status = http::SC_OK;

			plan.headers["Content-Type"] = http::Data::getMimeType("html");
			plan.headers["Content-Length"] = str::toString(body.size());

			plan.body = new MemoryBodySource(body);

			closedir(dir);
			return (plan);
		}
	}


	return (plan);
}