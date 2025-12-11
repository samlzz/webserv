/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 21:09:10 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/11 14:58:11 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <set>
#include <string>
#include <utility>

#include "Config.hpp"
#include "ftpp/AstNode.hpp"
#include "ftpp/Grammar.hpp"
#include "parsing/RawConfig.hpp"
#include "parsing/configParse.hpp"
#include "validation/configValidate.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

Config::Config(const std::string &configPath): _servs()
{
	Grammar	peg("assets/webserv.peg", true);
	AstNode	*ast = peg.parseFile(configPath, "config");

	if (!ast)
		throw config_parse::WsParseError(
			"Failed to parse config file '" + configPath + "'"
		);
	try
	{
		parseConfigFile(ast);
	}
	catch (...)
	{
		delete ast;
		throw;
	}
	delete ast;
}

Config::~Config() {}

// ============================================================================
// Accessors
// ============================================================================

const std::vector<Config::Server>	&Config::getServers(void) const
{
	return _servs;
}

const Config::Server	*Config::getServer(size_t idx) const
{
	if (idx >= _servs.size())
		return 0;
	return &_servs[idx];
}

const Config::Server	*Config::findServer(const std::string &host, uint16_t port) const
{
	for (size_t i = 0; i < _servs.size(); ++i)
	{
		if (_servs[i].host == host && _servs[i].port == port)
			return &_servs[i];
	}
	return NULL;
}

// ============================================================================
// Static methods
// ============================================================================

void	Config::assertNode(
	const AstNode *node,
	const std::string &expectedType,
	const std::string &level
)
{
	const std::string	&type = node->type();
	if (type == expectedType)
		return;

	throw config_parse::WsParseError(
		"Ast error: unexpected node(" + type + ")"
		+ " at " + level + " level, please check grammar"
	);
}

void	Config::assertProp(
	const AstNode *node,
	const std::string &propKey,
	const std::string &parent
)
{
	if (node->hasAttr(propKey))
		return;
	throw config_parse::WsParseError(
		"Ast error: property " + propKey
		+ " not found in " + parent + ", please check grammar"
	);
}

// ============================================================================
// Methods
// ============================================================================

void	Config::parseConfigFile(const AstNode *root)
{
	const std::vector<AstNode *> &children = root->children();
	if (children.empty())
		throw config_parse::WsParseError("Config file contains no server blocks");

	for (size_t i = 0; i < children.size(); ++i)
	{
		AstNode	*child = children[i];
		if (!child)
			continue;
		assertNode(child, "server", "config");

		config_parse::RawServer	raw = config_parse::extractServer(child);
		_servs.push_back(raw.normalize(ServerDefaults()));
	}
	validateConfig();
}

void	Config::validateConfig(void)
{
	std::set<std::pair<std::string, uint16_t> >	hostports;

	for (size_t i = 0; i < _servs.size(); ++i)
	{
		const Server	&s = _servs[i];

		config_validate::validateServerBasics(s);

		std::pair<std::string, uint16_t> hp(s.host, s.port);
		if (!hostports.insert(hp).second)
			throw config_validate::ServerError(
				s.host, s.port, "duplicate listen directive");

		config_validate::validateServerLocations(s);
	}
}