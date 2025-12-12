/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configValidate.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 22:19:52 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/12 18:31:51 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CONFIGVALIDATE_HPP__
# define __CONFIGVALIDATE_HPP__

# include <arpa/inet.h>
# include <netinet/in.h>
# include <sstream>
# include <stdexcept>
# include <stdint.h>

# include "config/Config.hpp"

/**
 * @brief Convert any streamable value to std::string.
 */
template <typename T>
inline std::string	toString(T value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

namespace config_validate
{

// ============================================================================
// Errors
// ============================================================================

/**
 * @brief Base class for all validation errors
 */
class WsConfigError : public std::runtime_error {
public:
	WsConfigError(const std::string &msg)
		: std::runtime_error("Config error: " + msg) {}
};

class ValueValidationError : public WsConfigError {
public:
	ValueValidationError(const std::string &msg)
		: WsConfigError(msg) {}
};

// ---- Server block errors ----

class ServerError : public WsConfigError {
public:
	ServerError(struct in_addr host, uint16_t port, const std::string &msg)
		: WsConfigError(std::string("server ") + inet_ntoa(host)
							+ ":" + toString(port) + ": " + msg)
	{}
};

// ---- Location block errors ----

class LocationError : public WsConfigError {
public:
	LocationError(const std::string &path, const std::string &msg)
		: WsConfigError("location '" + path + "': " + msg) {}
};

class LocationConflictError : public LocationError {
public:
	LocationConflictError(const std::string &path, const std::string &msg)
		: LocationError(path, "conflict: " + msg) {}
};

// ============================================================================
// Validation
// ============================================================================

void	validateServerBasics(const Config::Server &s);
void	validateServerLocations(const Config::Server &serv);

// ---- Props validation ----

enum e_path_rule
{
	PR_NONE				= 0,
	PR_MUST_EXIST		= 1 << 0,
	PR_MUST_DIR			= 1 << 1,
	PR_MUST_FILE		= 1 << 2,
	PR_MUST_EXEC		= 1 << 3,
	PR_NO_DOTDOT		= 1 << 4,
	PR_NO_REGEX			= 1 << 5,
	PR_NO_SLASH			= 1 << 6,
	PR_MUST_START_SL	= 1 << 7,
};

void	validatePathGeneric(const std::string &path, int32_t rules,
							const std::string &what);

} // namespace config_validate

#endif /* __CONFIGVALIDATE_HPP__ */