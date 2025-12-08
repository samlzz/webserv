/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseUtils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:29:07 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/08 19:13:22 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdint.h>

#include "Http/HttpTypes.hpp"

uint64_t	parseUnsigned(const std::string &s, const std::string &what)
{
	char		*endptr = 0;
	const char	*cstr = s.c_str();
	uint64_t	val = std::strtoul(cstr, &endptr, 10);

	if (endptr == cstr || *endptr != '\0')
		throw std::runtime_error("Invalid " + what + " value: '" + s + "'");
	return val;
}

// Parse bool "on/off true/false 1/0"
bool	parseBool(const std::string &s)
{
	if (s == "on" || s == "true" || s == "1")
		return true;
	if (s == "off" || s == "false" || s == "0")
		return false;
	throw std::runtime_error("Invalid boolean value: '" + s + "'");
}

// Parse size with suffix K/M/G (base 1024)
size_t	parseSize(const std::string &s)
{
	if (s.empty())
		throw std::runtime_error("Empty size value");

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

http::e_method	parseMethod(const std::string &s)
{
	if (s == "GET")     return http::MTH_GET;
	if (s == "HEAD")    return http::MTH_HEAD;
	if (s == "POST")    return http::MTH_POST;
	if (s == "PUT")     return http::MTH_PUT;
	if (s == "DELETE")  return http::MTH_DELETE;
	throw std::runtime_error("Unsupported HTTP method: '" + s + "'");
}

std::vector<std::string>	splitWords(const std::string &s)
{
	std::vector<std::string>	out;
	std::istringstream			iss(s);
	std::string					token;

	while (iss >> token)
		out.push_back(token);
	return out;
}