/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RawConfig.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 16:28:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/05 15:29:09 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RAW_CONFIG_HPP__
# define __RAW_CONFIG_HPP__

# include <map>
# include <stdint.h>

# include "config/Config.hpp"
# include "config/Optionnal.hpp"

namespace config_parse
{

// ============================================================================
// RawServer configuration
// ============================================================================
struct RawServer
{
	// ============================================================================
	// RawLocation configuration
	// ============================================================================
	struct RawLocation
	{
		std::string						path;

		/* Optionnal configuration */
		std::vector<http::e_method>		methods;
		Optionnal<std::string>			root;
		Optionnal<std::string>			index;
		Optionnal<bool>					autoindex;
		Config::t_errPages				errorPages;
		Optionnal<std::string>			defaultErrPage;
		Config::t_dict					cgiExts;

		Optionnal<std::string>			uploadPath;
		Optionnal<Config::StatusPath>	redirect;
		Optionnal<bool> 				sessionLogin;

		RawLocation(const std::string &pPath);
		Config::Server::Location
			normalize(const RawServer &parent, const Config::ServerDefaults &def);

	private:
		// forbidden
		RawLocation();
	};

// ============================================================================
// RawServer properties
// ============================================================================

	Optionnal<std::string>		host;
	Optionnal<uint16_t>			port;
	Optionnal<size_t>			maxBodySize;

	/* Default values for location blocks */
	std::vector<http::e_method>	d_methods;
	Optionnal<std::string>		d_root;
	Optionnal<std::string>		d_index;
	Optionnal<bool>				d_autoindex;
	Config::t_errPages			d_errorPages;
	Optionnal<std::string>		d_defaultErrPage;
	Config::t_dict				d_cgiExts;

	/* RawLocation blocks */
	std::vector<RawLocation>	locations;

	Config::Server	normalize(const Config::ServerDefaults &def);
};

// ============================================================================
// Helpers
// ============================================================================

template<typename K, typename V>
std::map<K, V>	concatMap(const std::map<K, V> &first, const std::map<K, V> &second)
{
	std::map<K, V> dest = first;
	dest.insert(second.begin(), second.end());
	return dest;
}

}

#endif /* __RAW_CONFIG_HPP__ */