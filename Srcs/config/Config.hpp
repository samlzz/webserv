/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 21:08:53 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/10 16:33:23 by sliziard         ###   ########.fr       */
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

# define MAX_MBODYSIZE	2147483648 // ? 2GB

// ============================================================================
// Global configuration
// ============================================================================

class Config {

public:
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

			std::vector<http::e_method>	methods;
			std::string					root;
			std::string					index;
			bool						autoindex;
			t_errPages					errorPages;
			std::string					defaultErrPage;
			t_dict						cgiExts;

			Optionnal<std::string>		uploadPath;
			Optionnal<StatusPath>		redirect;

			bool operator<(const Location &other) const;
		};

	// ============================================================================
	// Server properties
	// ============================================================================

		std::string				host;
		uint16_t				port; // According to RFC 793, the port is a 16 bit unsigned int.
		size_t					maxBodySize;

		/* Location blocks */
		std::vector<Location>	locations;

	// ============================================================================
	// Server methods
	// ============================================================================
		const Location	*findLocation(const std::string &path) const;
	};
	// ============================================================================
	// Default Server values
	// ============================================================================
	struct ServerDefaults
	{
		static const char 			*host;
		static const uint16_t		port;
		static const size_t			maxBodySize;

		static const http::e_method	methods[3];
		static const char			*root;
		static const char			*index;
		static const bool			autoindex;
		static const char			*defaultErrPage;
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
	std::vector<Server>			_servs;

	// ========================================================================
	// Internal parsing steps
	// ========================================================================

	void	parseConfigFile(const AstNode *root);
	void	validateConfig(void);

	// forbiden
	Config();
	Config& operator=(const Config &other);
	Config(const Config &other);
};

#endif /* __CONFIG_HPP__ */