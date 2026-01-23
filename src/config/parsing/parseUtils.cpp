/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseUtils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:29:07 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/23 20:42:25 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <stdint.h>

#include "http/HttpTypes.hpp"
#include "configParse.hpp"

namespace config_parse
{

// ========================================================================
// Generic helper
// ========================================================================

std::vector<std::string>	splitWords(const std::string &s)
{
	std::vector<std::string>	out;
	std::istringstream			iss(s);
	std::string					token;

	while (iss >> token)
		out.push_back(token);
	return out;
}

// ========================================================================
// Parse Numbers
// ========================================================================

uint64_t	parseUnsigned(const std::string &s, const std::string &what)
{
	char		*endptr = 0;
	const char	*cstr = s.c_str();
	uint64_t	val = std::strtoul(cstr, &endptr, 10);

	if (endptr == cstr || *endptr != '\0')
		throw ValueError(what, s);
	return val;
}

// Parse size with suffix K/M/G (base 1024)
size_t	parseSize(const std::string &s)
{
	if (s.empty())
		throw ValueError("size");

	uint64_t	multiplier = 1;
	std::string	numStr = s;
	char		last = std::toupper(s[s.size() - 1]);

	if (last == 'K')
	{
		multiplier = 1024UL;
		numStr = s.substr(0, s.size() - 1);
	}
	else if (last == 'M')
	{
		multiplier = 1024UL * 1024UL;
		numStr = s.substr(0, s.size() - 1);
	}
	else if (last == 'G')
	{
		multiplier = 1024UL * 1024UL * 1024UL;
		numStr = s.substr(0, s.size() - 1);
	}

	uint64_t	base = parseUnsigned(numStr, "size");
	return static_cast<size_t>(base * multiplier);
}

// ========================================================================
// Assign string to it's value
// ========================================================================

// Parse bool "on/off true/false 1/0"
bool	parseBool(const std::string &s)
{
	if (s == "on" || s == "true" || s == "1")
		return true;
	if (s == "off" || s == "false" || s == "0")
		return false;
	throw ValueError("boolean", s);
}

http::e_method	parseMethod(const std::string &s)
{
	if (s == "GET")     return http::MTH_GET;
	if (s == "HEAD")    return http::MTH_HEAD;
	if (s == "POST")    return http::MTH_POST;
	if (s == "PUT")     return http::MTH_PUT;
	if (s == "DELETE")  return http::MTH_DELETE;
	throw ValueError("HTTP method", s);
}

// ========================================================================
// Assign number to it's Status Code
// ========================================================================

http::e_status_code	parseStatusCode(const std::string &s)
{
	uint64_t	code = parseUnsigned(s, "status code");

	switch (code)
	{
		case 200: return http::SC_OK;
		case 201: return http::SC_CREATED;
		case 204: return http::SC_NO_CONTENT;
		case 301: return http::SC_MOVED_PERMANENTLY;
		case 302: return http::SC_FOUND;
		case 400: return http::SC_BAD_REQUEST;
		case 403: return http::SC_FORBIDDEN;
		case 404: return http::SC_NOT_FOUND;
		case 405: return http::SC_METHOD_NOT_ALLOWED;
		case 411: return http::SC_LENGTH_REQUIRED;
		case 413: return http::SC_CONTENT_TOO_LARGE;
		case 414: return http::SC_URI_TOO_LONG;
		case 500: return http::SC_INTERNAL_SERVER_ERROR;
		case 501: return http::SC_NOT_IMPLEMENTED;
		default:
			throw ValueError("status code", s);
	}
}

} // namespace config_parse