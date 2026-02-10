/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:05:13 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/07 17:48:07 by sliziard         ###   ########.fr       */
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
#include "utils/pathUtils.hpp"
#include "utils/stringUtils.hpp"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <vector>


CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}

// Create an uppered string
std::string		toUpperEnv(const std::string& pStr)
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

static inline std::vector<std::string>	genEnvp(const routing::Context& route, const HttpRequest& req)
{
	std::vector<std::string>	envp;

	addEnv(envp, "GATEWAY_INTERFACE", "CGI/1.1");
	addEnv(envp, "SERVER_PROTOCOL", "HTTP/1.1");
	addEnv(envp, "SERVER_SOFTWARE", "Webserv/1.0");

	addEnv(envp, "PATH_INFO", path::subInfo(route.normalizedPath));
	addEnv(envp, "PATH_TRANSLATED", route.location->root + path::subInfo(route.normalizedPath));
	addEnv(envp, "QUERY_STRING", req.getQuery());

	addEnv(envp, "REQUEST_METHOD", str::toString(req.getMethod()));
	addEnv(envp, "SCRIPT_NAME", path::subPath(route.normalizedPath));
	addEnv(envp, "SCRIPT_FILENAME", route.location->root + path::subPath(route.normalizedPath));

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

	http::t_headers headers = req.getHeaders();
	http::t_headers::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
		if (it->first == "Content-Type" || it->first == "Content-Length")
			continue;
		addEnv(envp, "HTTP_" + toUpperEnv(it->first), it->second);
	}

	addEnv(envp, "SESSION_USERNAME", route.session->username);
	return envp;
}

// Generate an vecor of string version of the argv for cgi
static inline std::vector<std::string>	genArgv(const routing::Context& route)
{
	std::vector<std::string>		vec;
	std::string						ext = path::subExt(
											path::subPath(route.normalizedPath)
										);
	Config::t_dict::const_iterator	it = route.location->cgiExts.find(ext);

	vec.push_back(it->second);
	vec.push_back(route.location->root + path::subPath(route.normalizedPath));

	return vec;
}

ResponsePlan CgiHandler::handle(
	const HttpRequest& req,
	const routing::Context& route
) const
{
	ResponsePlan plan;

	CgiOutputParser* parser = new CgiOutputParser();
	CgiProcess* process = new CgiProcess(*parser);
	process->retain();

	IConnection* readConn = process->start(
		genArgv(route),
		genEnvp(route, req),
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

	CgiBodySource* body = new CgiBodySource(process, parser);
	process->release();

	plan.status = http::SC_OK;
	plan.body = body;
	plan.event = ConnEvent::spawn(readConn);

	return plan;
}
