/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configValidate.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 22:19:52 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/11 14:23:18 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CONFIGVALIDATE_HPP__
# define __CONFIGVALIDATE_HPP__

#include "config/Config.hpp"
#include <sstream>
#include <stdexcept>
# include <stdint.h>

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
		: std::runtime_error(msg) {}
};

class ValueValidationError : public WsConfigError {
public:
	ValueValidationError(const std::string &msg)
		: WsConfigError(msg) {}
};

// ---- Server block errors ----

class ServerError : public WsConfigError {
public:
	ServerError(const std::string &host, uint16_t port, const std::string &msg)
		: WsConfigError("server " + host + ":" + toString(port) + ": " + msg) {}
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

void	validateServer(const Config::Server &serv);

} // namespace config_validate

#endif /* __CONFIGVALIDATE_HPP__ */