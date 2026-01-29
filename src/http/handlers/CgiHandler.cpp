/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:05:13 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 03:29:36 by achu             ###   ########.fr       */
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

#include <cstring>

#include "utils/String.hpp"

CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}

// Create a content-value string for the header "Allow"
static inline std::string	methodTOstring(const http::e_method& pMethod) {
	switch (pMethod) {
		case http::GET:    return "GET";
		case http::POST:   return "POST";
		case http::DELETE: return "DELETE";
		case http::PUT:    return "PUT";
		case http::HEAD:   return "HEAD";
		default:           return "UNKNOWN";
	}
}

// Substract the extension part of a URI path or file
static inline std::string	subExt(const std::string& pPath)
{
	std::string		result;
	size_t			start;
	size_t			end;

	start = pPath.find_last_of('.');
	if (start == std::string::npos)
		return "";

	end = pPath.find_first_of('/', start);
	if (end == std::string::npos)
		end = pPath.length();

	result = pPath.substr(start, end - start);
	if (result.empty())
		return "";

	return result;
}

static inline std::string	subPath(const std::string& pPath)
{
	std::string		result;
	size_t			end;

	end = pPath.find_last_of('.');
	if (end == std::string::npos)
		return pPath;

	end = pPath.find_first_of('/', end);
	if (end == std::string::npos)
		return pPath;

	result = pPath.substr(0, end);
	if (result.empty())
		return "";

	return result;
}

static inline std::string	subInfo(const std::string& pPath)
{
	std::string		result;
	size_t			start;

	start = pPath.find_last_of('.');
	if (start == std::string::npos)
		return "";

	start = pPath.find_first_of('/', start);
	if (start == std::string::npos)
		return "";

	result = pPath.substr(start, pPath.length() - start);
	if (result.empty())
		return "";

	return result;
}

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
	std::vector<std::string> envp;

	addEnv(envp, "GATEWAY_INTERFACE", "CGI/1.1");
	addEnv(envp, "REQUEST_METHOD", methodTOstring(req.getMethod()));
	addEnv(envp, "QUERY_STRING", req.getQuery());
	addEnv(envp, "PATH_INFO", subInfo(req.getPath()));
	addEnv(envp, "PATH_TRANSLATED", route.location->root + getInfo(req.getPath()));
	addEnv(envp, "SCRIPT_NAME", subPath(req.getPath()));
	addEnv(envp, "SCRIPT_FILENAME", route.location->root + subPath(req.getPath());
	addEnv(envp, "REDIRECT_STATUS", "200");

	addEnv(envp, "REMOTE_ADDR", "127.0.0.1");
	addEnv(envp, "SERVER_PORT", "8080");
	addEnv(envp, "SERVER_NAME", "localhost");
	addEnv(envp, "SERVER_PROTOCOL", "HTTP/1.1");
	addEnv(envp, "SERVER_SOFTWARE", "Webserv/1.0");

	if (req.hasHeader("Content-Length"))
		addEnv(envp, "CONTENT_LENGTH", req.getHeader("Content-Length"));
	else
		addEnv(envp, "CONTENT_LENGTH", utils::toString(req.getBody().size()));
	
	if (req.hasHeader("Content-Type"))
		addEnv(envp, "CONTENT_TYPE", req.getHeader("Content-Type"));

	http::t_headers headers = req.getHeaders();
	http::t_headers::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
		if (it->first == "Content-Type" || it->first == "Content-Length")
			continue;
		addEnv(envp, "HTTP_" + toUpperEnv(it->first), it->second);
	}

	return envp;
}

// Generate an vecor of string version of the argv for cgi
static inline std::vector<std::string>	genArgv(const routing::Context& route, const HttpRequest& req)
{
	std::vector<std::string> vec;

	Config::t_dict::const_iterator	it = route.location->cgiExts.find(subExt(req.getPath()));
	if (it != _location->cgiExts.end())
		vec.push_back(it->second);

	vec.push_back(route.location->root + req.getPath());

	return vec;
}

// Takes a vector of string and convert it to an allocated array of char ptr
static inline char**	toCharArray(const std::vector<std::string>& pVec)
{
	char** tab = new char*[pVec.size() + 1];
	for (size_t i = 0; i < pVec.size(); i++)
		tab[i] = strdup(pVec[i].c_str());
	tab[pVec.size()] = NULL;
	return tab;
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
		getScriptPath(req),
		getArgv(req),
		getEnvp(req),
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
