/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 21:08:53 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/08 17:17:35 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# include <cstddef>
# include <map>
# include <stdint.h>
# include <string>
# include <vector>

#include "Optionnal.hpp"
# include "ftpp/AstNode.hpp"

// ============================================================================
// Global configuration
// ============================================================================

class Config {

public:
	Config();
	Config(const std::string &configPath);
	~Config();

	// ============================================================================
	// Temp Enum
	// TODO: retrieve htpp namespace
	// ============================================================================

	enum	e_method
		{ MTH_GET
		, MTH_HEAD
		, MTH_POST
		, MTH_PUT
		, MTH_DELETE
		, MTH_UNKNOWN
	};

	enum	e_status_code
		{ SC_OK = 200
		, SC_CREATED = 201
		, SC_NO_CONTENT = 204
		, SC_MOVED_PERMANENTLY = 301
		, SC_FOUND = 302
		, SC_BAD_REQUEST = 400
		, SC_FORBIDDEN = 403
		, SC_NOT_FOUND = 404
		, SC_METHOD_NOT_ALLOWED = 405
		, SC_LENGTH_REQUIRED = 411
		, SC_CONTENT_TOO_LARGE = 413
		, SC_URI_TOO_LONG = 414
		, SC_INTERNAL_SERVER_ERROR = 500
		, SC_NOT_IMPLEMENTED = 501
	};


	// ============================================================================
	// Server configuration
	// ============================================================================

	struct Server
	{
		// ============================================================================
		// Location configuration
		// ============================================================================
		struct Location
		{
			std::string											path;

			/* Optionnal configuration */
			Optionnal<std::vector<e_method> >					methods;
			Optionnal<std::string>								root;
			Optionnal<std::string>								index;
			Optionnal<bool>										autoindex;
			Optionnal<std::map<e_status_code, std::string> >	errorPages;
			Optionnal<std::map<std::string, std::string> >		cgiExts;

			Optionnal<std::string>								cgiPath;
			Optionnal<std::string>								uploadPath;
			Optionnal<std::string>								redirectPath;
			Optionnal<e_status_code>							redirectStatus;

			Location();

			std::string	getErrorPage(const Server &parent, e_status_code code) const;
			bool		isAllowed(const Server &parent, e_method method) const;
		};

	// ============================================================================
	// Server properties
	// ============================================================================

		std::string								host;
		uint16_t								port; // According to RFC 793, the port is a 16 bit unsigned int.
		size_t									maxBodySize;

		/* Default values for location blocks */
		std::vector<e_method>					d_methods;
		std::string								d_root;
		std::string								d_index;
		bool									d_autoindex;
		std::map<e_status_code, std::string>	d_errorPages;
		std::map<std::string, std::string>		d_cgiExts;

		/* Location blocks */
		std::vector<Location>					locations;

	// ============================================================================
	// Server methods
	// ============================================================================
		Server();

		const Location				*findLocation(const std::string &path) const;
	};

	// ============================================================================
	// Global configuration methods
	// ============================================================================

	const std::vector<Server>	&getServers(void) const;
	const Server				*getServer(size_t idx) const;
	const Server				*findServer(const std::string &host,
											uint16_t port) const;

private:
	// ============================================================================
	// Internal data
	// ============================================================================

	AstNode				*_ast;
	std::string			_path;
	std::vector<Server>	_servs;

	// ========================================================================
	// Internal parsing steps
	// ========================================================================

	void	parseConfigFile(void);
	void	validateConfig(void);

	// ---- Extract from AST ----
	void	extractServer(AstNode *root);
	void	extractServerDirectives(AstNode *server);
	void	extractLocations(AstNode *server);

	// forbiden copy
	Config& operator=(const Config &other);
	Config(const Config &other);
};

#endif /* __CONFIG_HPP__ */