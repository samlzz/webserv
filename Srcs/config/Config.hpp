/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 21:08:53 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/10 03:02:58 by sliziard         ###   ########.fr       */
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

	// ============================================================================
	// Configuration types
	// ============================================================================
	struct StatusPath
	{
		http::e_status_code	code;
		std::string			path;

		StatusPath(): code(http::SC_NONE), path() {}
	};
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
			std::string					path;

			/* Optionnal configuration */
			std::vector<http::e_method>	methods;
			Optionnal<std::string>		root;
			Optionnal<std::string>		index;
			Optionnal<bool>				autoindex;
			t_errPages					errorPages;
			Optionnal<std::string>		defaultErrPage;
			t_dict						cgiExts;

			Optionnal<std::string>		uploadPath;
			Optionnal<StatusPath>		redirect;

			Location();
			bool operator<(const Location &other) const;

			Location	resolve(const Server &parent); // TODO: not implemented
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
		std::string					d_defaultErrPage;
		t_dict						d_cgiExts;

		/* Location blocks */
		std::vector<Location>		locations;

	// ============================================================================
	// Server methods
	// ============================================================================
		Server();

		const Location	*findLocation(const std::string &path) const;
	};

	// ============================================================================
	// Global configuration methods
	// ============================================================================

	const std::vector<Server>	&getServers(void) const;
	const Server				*getServer(size_t idx) const;
	const Server				*findServer(const std::string &host,
											uint16_t port) const;

	// ============================================================================
	// Static helpers
	// ============================================================================
	static void					assertNode(const AstNode *node,
											const std::string &expectedType,
											const std::string &level);
	static void					assertProp(const AstNode *node,
											const std::string &propKey,
											const std::string &parent);

private:
	// ============================================================================
	// Internal data
	// ============================================================================
	std::vector<Server>	_servs;

	// ========================================================================
	// Internal parsing steps
	// ========================================================================

	void	parseConfigFile(const AstNode *root);
	void	validateConfig(const AstNode *root);

	// forbiden copy
	Config& operator=(const Config &other);
	Config(const Config &other);
};

#endif /* __CONFIG_HPP__ */