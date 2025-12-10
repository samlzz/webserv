/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 09:36:59 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/10 17:04:59 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <string>
#include <vector>

#include "Http/HttpTypes.hpp"
#include "ftpp/AstNode.hpp"
#include "config/Config.hpp"
#include "./configParse.hpp"
#include "./parseUtils.hpp"
#include "./RawConfig.hpp"

namespace config_parse
{

// ========================================================================
// Low-level Extraction Helpers
// ========================================================================

// both server and location
static std::vector<http::e_method>	_extractMethods(const std::string &s)
{
	std::vector<http::e_method>	methods;
	std::vector<std::string>	toks = splitWords(s);

	size_t i = 0;
	while (i < toks.size())
		methods.push_back(parseMethod(toks[i++]));
	return methods;
}

// server only
static uint64_t	_extractPort(const std::string &s)
{
	return parseUnsigned(s, "port");
}

// location only
static Config::StatusPath	_extractReturn(const std::string &s)
{
	Config::StatusPath			redirect;
	std::vector<std::string>	toks = splitWords(s);

	if (toks.size() > 2)
		throw ValueError("redirect", s);
	redirect.code = parseStatusCode(toks[0]);
	redirect.path = toks[1];
	return redirect;
}

// ========================================================================
// Composite Directive Handlers (error_page, cgi)
// ========================================================================

static inline void	_appendErrorsPageItem(Config::t_errPages &errPages,
											Optionnal<std::string> &defaultPage,
											const AstNode *errorPageNode)
{
	Config::assertProp(errorPageNode, "path", "error_page");
	std::vector<std::string> toks = splitWords(
		errorPageNode->getAttr("codes", "")
	);

	if (toks.empty())
		return fillDest(defaultPage, errorPageNode, "path");

	std::string	path = errorPageNode->getAttr("path", "");
	for (size_t i = 0; i < toks.size(); ++i)
		errPages[parseStatusCode(toks[i])] = path;
}

static inline void	_appendCgiItem(Config::t_dict &exts,
									const AstNode *cgiNode)
{
	Config::assertProp(cgiNode, "extension", "cgi");
	Config::assertProp(cgiNode, "path", "cgi");

	exts[cgiNode->getAttr("extension", "")] = cgiNode->getAttr("path", "");
}

// ========================================================================
// Location Block Extraction
// ========================================================================

RawServer::RawLocation	extractLocation(const AstNode *locationNode)
{
	RawServer::RawLocation	loc;

	fillDest(loc.path, locationNode, "path");
	fillDest(loc.methods, locationNode, "methods", _extractMethods);
	fillDest(loc.root, locationNode, "root");
	fillDest(loc.index, locationNode, "index");
	fillDest(loc.autoindex, locationNode, "autoindex", parseBool);
	fillDest(loc.uploadPath, locationNode, "upload");
	fillDest(loc.redirect, locationNode, "redirect", _extractReturn);

	const std::vector<AstNode *> &children = locationNode->children();
	for (size_t i = 0; i < children.size(); ++i)
	{
		AstNode	*prop = children[i];

		if (!prop)
			continue;
		if (prop->type() == "cgi")
			_appendCgiItem(loc.cgiExts, prop);
		else if (prop->type() == "error_page")
			_appendErrorsPageItem(loc.errorPages, loc.defaultErrPage, prop);
	}
	return loc;
}

// ========================================================================
// Server Block Extraction
// ========================================================================

static inline void	_extractServDirectives(RawServer &serv, const AstNode *serverNode)
{
	fillDest(serv.host, serverNode, "host");
	fillDest(serv.port, serverNode, "port", _extractPort);
	fillDest(serv.maxBodySize, serverNode, "client_max_body_size", parseSize);
	fillDest(serv.d_methods, serverNode, "methods", _extractMethods);
	fillDest(serv.d_root, serverNode, "root");
	fillDest(serv.d_index, serverNode, "index");
	fillDest(serv.d_autoindex, serverNode, "autoindex", parseBool);
}

static inline void	_extractServChildren(RawServer &serv, const AstNode *serverNode)
{
	const std::vector<AstNode *> &children = serverNode->children();
	for (size_t i = 0; i < children.size(); ++i)
	{
		AstNode	*child = children[i];

		if (!child)
			continue;
		if (child->type() == "location")
			serv.locations.push_back(extractLocation(child));
		else if (child->type() == "cgi")
			_appendCgiItem(serv.d_cgiExts, child);
		else if (child->type() == "error_page")
			_appendErrorsPageItem(serv.d_errorPages, serv.d_defaultErrPage, child);
	}
}

RawServer	extractServer(const AstNode *serverNode)
{
	RawServer	srv;

	_extractServDirectives(srv, serverNode);
	_extractServChildren(srv, serverNode);
	return srv;
}

} // namespace config_parse