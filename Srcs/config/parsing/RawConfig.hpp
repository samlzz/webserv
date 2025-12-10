/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RawConfig.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 16:28:10 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/10 16:35:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RAWCONFIG_HPP__
# define __RAWCONFIG_HPP__

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

		Config::Server::Location
			normalize(const RawServer &parent, const Config::ServerDefaults &def); // TODO: not implemented
	};

// ============================================================================
// RawServer properties
// ============================================================================

	Optionnal<std::string>				host;
	Optionnal<uint16_t>					port;
	Optionnal<size_t>					maxBodySize;

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

	Config::Server	normalize(const Config::ServerDefaults &def); // TODO: not implemented
};

}

#endif /* __RAWCONFIG_HPP__ */