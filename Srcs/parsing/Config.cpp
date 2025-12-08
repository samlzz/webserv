/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 21:09:10 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/07 21:43:25 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>

#include "ftpp/Grammar.hpp"
#include "Config.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

Config::Config(): _ast(NULL), _path(""), _servs()
{}


Config::Config(const std::string &configPath)
	: _ast(NULL), _path(configPath), _servs()
{
	parseConfigFile();
}

Config::~Config()
{
	delete _ast;
}

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
// Methods
// ============================================================================

void	Config::parseConfigFile(void)
{
	Grammar	peg("assets/webserv.peg", true);

	_ast = peg.parseFile(_path);
	// TODO: fill structs with _ast data
	validateConfig();
}

void	Config::validateConfig(void)
{
	// TODO: check if configurations values are not
}