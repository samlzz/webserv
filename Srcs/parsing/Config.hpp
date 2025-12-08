/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 21:08:53 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/08 18:05:01 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# include <cstddef>
# include <map>
# include <stdint.h>
# include <string>
# include <vector>

# include "Optionnal.hpp"
# include "Http/HttpTypes.hpp"
# include "ftpp/AstNode.hpp"

// ============================================================================
// Global configuration
// ============================================================================

class Config {

public:
	Config();
	Config(const std::string &configPath);
	~Config();

	typedef std::map<http::e_status_code, std::string>	t_errPages;
	typedef std::map<std::string, std::string>			t_dict;

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
			Optionnal<std::vector<http::e_method> >	methods;
			Optionnal<std::string>					root;
			Optionnal<std::string>					index;
			Optionnal<bool>							autoindex;
			Optionnal<t_errPages>					errorPages;
			Optionnal<t_dict>						cgiExts;

			Optionnal<std::string>					cgiPath;
			Optionnal<std::string>					uploadPath;
			Optionnal<std::string>					redirectPath;
			Optionnal<http::e_status_code>			redirectStatus;

			Location();

			std::string	getErrorPage(const Server &parent, http::e_status_code code) const;
			bool		isAllowed(const Server &parent, http::e_method method) const;
		};

	// ============================================================================
	// Server properties
	// ============================================================================

		std::string					host;
		uint16_t					port; // According to RFC 793, the port is a 16 bit unsigned int.
		size_t						maxBodySize;

		/* Default values for location blocks */
		std::vector<http::e_method>	d_methods;
		std::string					d_root;
		std::string					d_index;
		bool						d_autoindex;
		t_errPages					d_errorPages;
		t_dict						d_cgiExts;

		/* Location blocks */
		std::vector<Location>		locations;

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