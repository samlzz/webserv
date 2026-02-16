/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:05:13 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/10 23:08:36 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "http/HttpTypes.hpp"
#include "http/cgi/CgiProcess.hpp"
#include "http/cgi/CgiOutputParser.hpp"
#include "bodySrcs/CgiBodySource.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/routing/Router.hpp"
#include "server/connections/ConnEvent.hpp"
#include "utils/fileSystemUtils.hpp"
#include "utils/pathUtils.hpp"
#include "utils/stringUtils.hpp"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>


CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}

static std::string		_toUpperEnv(const std::string& pStr)
{
	unsigned char ch;
	std::string upper;

	for (size_t i = 0; i < pStr.length(); i++) {
		ch = static_cast<unsigned char>(pStr[i]);
		if (ch == '-')
			upper.push_back('_');
		else
			upper.push_back(std::toupper(ch));
	}

	return upper;
}

static inline void	addEnv(std::vector<std::string>& pVec, const std::string& pKey, const std::string& pValue)
{
	pVec.push_back(pKey + "=" + pValue);
}

static inline std::vector<std::string>	genEnvp(
											const std::string &scriptName,
											const HttpRequest& req,
											const routing::Context& route
										)
{
	std::vector<std::string>	envp;
	std::string					pathInfo = path::subInfo(route.normalizedPath);

	addEnv(envp, "GATEWAY_INTERFACE", "CGI/1.1");
	addEnv(envp, "SERVER_PROTOCOL", "HTTP/1.1");
	addEnv(envp, "SERVER_SOFTWARE", "Webserv/1.0");

	addEnv(envp, "PATH_INFO", pathInfo);
	addEnv(envp, "PATH_TRANSLATED", route.location->root + pathInfo);
	addEnv(envp, "QUERY_STRING", req.getQuery());

	addEnv(envp, "REQUEST_METHOD", str::toString(req.getMethod()));
	addEnv(envp, "SCRIPT_NAME", scriptName);
	addEnv(envp, "SCRIPT_FILENAME", route.location->root + scriptName);

	std::string	remoteAddr = route.remote ? route.remote->addr : "";
	std::string	serverPort = route.local ? str::toString(route.local->port) : "";
	std::string	serverName = route.local ? route.local->addr : "";

	addEnv(envp, "REMOTE_ADDR", remoteAddr);
	addEnv(envp, "SERVER_PORT", serverPort);
	addEnv(envp, "SERVER_NAME", serverName);

	std::string	contentLength = req.hasField("Content-Length")
									? req.getField("Content-Length")
									: str::toString(req.getBody().size());
	addEnv(envp, "CONTENT_LENGTH", contentLength);
	
	if (req.hasField("Content-Type"))
		addEnv(envp, "CONTENT_TYPE", req.getField("Content-Type"));

	http::t_headers					headers = req.getHeaders();
	http::t_headers::const_iterator	it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		if (it->first == "Content-Type" || it->first == "Content-Length")
			continue;
		addEnv(envp, "HTTP_" + _toUpperEnv(it->first), it->second);
	}

	addEnv(envp, "SESSION_USERNAME", route.currSession ? route.currSession->username : "");
	return envp;
}

// Generate an vecor of string version of the argv for cgi
static inline std::vector<std::string>	genArgv(const std::string &scriptName, const routing::Context& route)
{
	std::vector<std::string>		vec;
	std::string						ext = path::subExt(scriptName);
	Config::t_dict::const_iterator	it = route.location->cgiExts.find(ext);

	vec.push_back(it->second);
	vec.push_back(route.location->root + scriptName);
	return vec;
}

ResponsePlan CgiHandler::handle(
	const HttpRequest& req,
	const routing::Context& route
) const
{
	std::string	scriptName = path::subPath(route.normalizedPath);
	std::string	scriptPath = route.location->root + scriptName;
	struct stat	st;

	if (!fs::isExist(scriptPath, &st))
		return ErrorBuilder::build(
			http::SC_NOT_FOUND,
			route.location
		);
	if (!fs::isFile(st) || access(scriptPath.c_str(), R_OK) != 0)
		return ErrorBuilder::build(
			http::SC_FORBIDDEN,
			route.location
		);

	CgiOutputParser	*parser = new CgiOutputParser();
	CgiProcess		*process = new CgiProcess(*parser);
	process->retain();

	try {
		IConnection* readConn = process->start(
			genArgv(scriptName, route),
			genEnvp(scriptName, req, route),
			req.getBody()
		);

		if (!readConn)
		{
			process->release();
			delete parser;
			return ErrorBuilder::build(
					http::SC_INTERNAL_SERVER_ERROR,
					route.location);
		}

		ResponsePlan	plan;

		plan.status = http::SC_OK;
		plan.event = ConnEvent::spawn(readConn);
		plan.body = new CgiBodySource(process, parser);;
		process->release();

		return plan;
	} catch (...) {
		delete process;
		delete parser;
		throw ;
	}
}
