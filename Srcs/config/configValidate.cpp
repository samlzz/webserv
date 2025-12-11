/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configValidate.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 22:28:48 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/11 14:20:48 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <set>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "config/Config.hpp"
#include "config/configValidate.hpp"

namespace config_validate
{

// ============================================================================
// Location properties validation
// ============================================================================

// forbid empty methods, a method MTH_UNKNOWN and duplicates
static inline void	validateMethods(const Config::Server::Location &loc)
{
	if (loc.methods.empty())
		throw LocationError(loc.path, "no HTTP methods declared");

	for (size_t i = 0; i < loc.methods.size(); ++i)
	{
		http::e_method m = loc.methods[i];
		if (m == http::MTH_UNKNOWN)
			throw LocationError(loc.path, "unknown HTTP method");

		for (size_t j = i + 1; j < loc.methods.size(); ++j)
		{
			if (m == loc.methods[j])
				throw LocationError(loc.path, "duplicate HTTP method");
		}
	}
}

// forbid SC_NONE status code and '..' in path or empty path
static inline void	validateErrorPages(const Config::Server::Location &loc)
{
	for (Config::t_errPages::const_iterator it = loc.errorPages.begin();
			it != loc.errorPages.end(); ++it)
	{
		if (it->first == http::SC_NONE)
			throw LocationError(loc.path, "invalid status code in error_page");

		validatePathGeneric(it->second, PR_NO_DOTDOT, "error_page path");
	}
}

// root must be defined
// extensions must not be empty and start with '.'
// paths must exist and be executable
static inline void	validateCgi(const Config::Server::Location &loc)
{
	if (loc.cgiExts.empty())
		return;

	if (loc.root.empty())
		throw LocationError(loc.path, "root required when CGI is used");

	for (Config::t_dict::const_iterator it = loc.cgiExts.begin();
			it != loc.cgiExts.end(); ++it)
	{
		const std::string &ext = it->first;

		if (ext.empty() || ext[0] != '.')
			throw LocationError(loc.path,
				"invalid cgi extension '" + ext + "'");

		validatePathGeneric(it->second, PR_MUST_EXIST | PR_MUST_EXEC,
			"CGI executable");
	}
}

// forbid other code that 301 or 302, and empty path
static inline void	validateRedirect(const Config::Server::Location &loc)
{
	if (!loc.redirect)
		return;
	const Config::StatusPath &r = *loc.redirect;

	if (r.code != http::SC_MOVED_PERMANENTLY && r.code != http::SC_FOUND )
		throw LocationError(loc.path,
			"invalid redirect status code (only 301/302 supported)");

	validatePathGeneric(r.path, PR_NONE, "redirect path");
}

// ============================================================================
// Location validation
// ============================================================================

/** forbid combinated:
 * - upload and cgi
 * - redirect and upload
 * - redirect and cgi
 */
static inline void	validateLocationConflicts(const Config::Server::Location &loc)
{
	if (loc.uploadPath && !loc.cgiExts.empty())
		throw LocationConflictError(loc.path,
			"cannot enable upload and CGI simultaneously");

	if (loc.redirect && loc.uploadPath)
		throw LocationConflictError(loc.path,
			"cannot use upload and redirect together");

	if (loc.redirect && !loc.cgiExts.empty())
		throw LocationConflictError(loc.path,
			"cannot use redirect and CGI together");
}

static void	validateLocation(const Config::Server::Location &loc)
{
	validatePathGeneric(
		loc.path,
		PR_MUST_START_SL | PR_NO_REGEX | PR_NO_DOTDOT,
		"location path"
	);
	validateMethods(loc);
	validatePathGeneric(loc.root, PR_NO_DOTDOT, "root");
	validatePathGeneric(
		loc.index, PR_NO_DOTDOT | PR_NO_START_SL, "index"
	);
	// validateAutoIndex ?
	validateErrorPages(loc);
	validatePathGeneric(
		loc.defaultErrPage, PR_MUST_EXIST | PR_MUST_FILE, "default error_page"
	);
	validateCgi(loc);
	if (loc.uploadPath)
		validatePathGeneric(
			*loc.uploadPath,
			PR_MUST_EXIST | PR_MUST_DIR,
			"upload path"
		);
	validateRedirect(loc);
	validateLocationConflicts(loc);
}

// ============================================================================
// Server Validation
// ============================================================================

static inline std::string	_normalizePath(const std::string &path)
{
	if (path.size() > 1 && path[path.size() - 1] == '/')
		return path.substr(0, path.size() - 1);
	return path;
}

static inline void	_validateServerBasics(const Config::Server &s)
{
	if (s.host.empty())
		throw ServerError(s.host, s.port, "host cannot be empty");
	if (s.port == 0)
		throw ServerError(s.host, s.port, "port cannot be 0");
	if (s.maxBodySize == 0 || s.maxBodySize > MAX_MBODYSIZE)
		throw ServerError(s.host, s.port, "invalid client_max_body_size");
}

void	validateServer(const Config::Server &serv)
{
	_validateServerBasics(serv);
	std::set<std::string>	normalizedPaths;

	for (size_t i = 0; i < serv.locations.size(); ++i)
	{
		const Config::Server::Location	&loc = serv.locations[i];
		std::string						np = _normalizePath(loc.path);

		if (!normalizedPaths.insert(np).second)
			throw LocationError(loc.path, "duplicate location path");
		validateLocation(loc);
	}
}

} // namespace config_validate