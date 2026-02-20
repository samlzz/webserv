/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigPrint.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 21:00:34 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/20 18:22:59 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifdef WS_CONFIG_DEBUG

# include <arpa/inet.h>
# include <cstddef>
# include <iostream>

# include "Config.hpp"

void Config::print(std::ostream &os) const
{
	os << "==================== CONFIG ====================\n";
	os << "Servers count: " << _servs.size() << "\n\n";

	for (size_t i = 0; i < _servs.size(); ++i)
	{
		const Server &srv = _servs[i];

		os << "------------------------------------------------\n";
		os << "SERVER #" << i << "\n";
		os << " host          : " << inet_ntoa(srv.host) << "(binary: " << srv.host.s_addr << ")\n";
		os << " port          : " << srv.port << "\n";
		os << " locations     : " << srv.locations.size() << "\n";

		// ============================================================
		// LOCATIONS
		// ============================================================
		for (size_t j = 0; j < srv.locations.size(); ++j)
		{
			const Server::Location &loc = srv.locations[j];

			os << "\n  >>> Location #" << j << "\n";
			os << "    path        : " << loc.path << "\n";

			// METHODS
			os << "    methods     : ";
			for (size_t k = 0; k < loc.methods.size(); ++k)
			{
				os << loc.methods[k];
				if (k + 1 < loc.methods.size()) os << ", ";
			}
			if (loc.methods.empty())
				os << "(none)";
			os << "\n";

			// OTHER SIMPLE FIELDS
			os << "    maxBodySize : " << loc.maxBodySize << "\n";
			os << "    root        : " << loc.root << "\n";
			os << "    index       : " << loc.index << "\n";
			os << "    autoindex   : " << (loc.autoindex ? "on" : "off") << "\n";

			// ERROR PAGES
			os << "    error_pages : ";
			if (loc.errorPages.empty())
			{
				os << "(none)\n";
			}
			else
			{
				os << "\n";
				for (Config::t_errPages::const_iterator it = loc.errorPages.begin();
						it != loc.errorPages.end(); ++it)
				{
					os << "        [" << it->first
						<< "] -> " << it->second << "\n";
				}
			}
			os << "    defaultErrPage : " << loc.defaultErrPage << "\n";

			// CGI
			os << "    cgiExts     : ";
			if (loc.cgiExts.empty())
			{
				os << "(none)\n";
			}
			else
			{
				os << "\n";
				for (Config::t_dict::const_iterator it = loc.cgiExts.begin();
						it != loc.cgiExts.end(); ++it)
				{
					os << "        " << it->first << " -> " << it->second << "\n";
				}
			}

			// UPLOAD (Optionnal)
			os << "    uploadPath  : ";
			if (loc.uploadPath.isSome())
				os << *loc.uploadPath << "\n";
			else
				os << "(none)\n";

			// REDIRECT (Optionnal)
			os << "    redirect    : ";
			if (loc.redirect.isSome())
			{
				const StatusPath &sp = *loc.redirect;
				os << "[" << sp.code << "] -> " << sp.path << "\n";
			}
			else
				os << "(none)\n";

			// SESSION LOGIN (Default to off)
			os << "    sessionLogin  : " << (loc.sessionLogin ? "on\n" : "off\n");

			// COOKIES SET
			os << "    cookiesSet  :";
			if (loc.cookiesSet.empty())
				os << "(none)\n";
			else
			{
				os << '\n';
				for (size_t i = 0; i < loc.cookiesSet.size(); ++i)
					os << "        " << loc.cookiesSet[i] << '\n';
			}
			
			// COOKIES VARY
			os << "    cookiesVary  :";
			if (loc.cookiesVary.empty())
				os << "(none)\n";
			else
			{
				os << '\n';
				for (size_t i = 0; i < loc.cookiesVary.size(); ++i)
					os << "        " << loc.cookiesVary[i] << '\n';
			}
		}
		os << "\n";
	}

	os << "================== END CONFIG ==================\n";
}

#endif // WS_CONFIG_DEBUG