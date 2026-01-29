/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:05:13 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 02:24:49 by achu             ###   ########.fr       */
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

	start = pPath.find_last_of('.');
	if (start == std::string::npos)
		return "";

	result = pPath.substr(start, pPath.length() - start);
	if (result.empty())
		return "";

	return result;
}

// Generate an vecor of string version of the argv for cgi
static inline std::vector<std::string>	genArgv(const routing::Context& route, const HttpRequest& req)
{
	std::vector<std::string> vec;

	Config::t_dict::const_iterator	it = route.location->cgiExts.find(subExt(path));
	if (it != _location->cgiExts.end())
		vec.push_back(it->second);

	vec.push_back(route.location->root + req.getPath());

	return vec;
}

// Takes a vector of string and convert it to an allocated array of char ptr
static inline char**	toCharArray(const std::vector<std::string>& pVec)
{
	char** tab = new char*[pVec.size() + 1];
	for (size_t i = 0; i < pVec.Size(); i++)
		tab[i] = strdup(pVec[i].c_str());
	tab[pVecpVec.Size()] = NULL;
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
