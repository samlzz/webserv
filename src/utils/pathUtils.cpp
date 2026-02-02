/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 12:21:59 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/31 12:23:47 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pathUtils.hpp"

namespace path
{

// -----------------------------------------------------------------------------
// subExt
// -----------------------------------------------------------------------------
std::string subExt(const std::string &filePath)
{
	size_t slash = filePath.find_last_of('/');
	size_t dot   = filePath.find_last_of('.');

	if (dot == std::string::npos)
		return "";

	if (slash != std::string::npos && dot < slash)
		return "";

	if (dot + 1 >= filePath.size())
		return "";

	return filePath.substr(dot + 1);
}

// -----------------------------------------------------------------------------
// subPath
// -----------------------------------------------------------------------------
std::string	subPath(const std::string& pPath)
{
	std::string		result;
	size_t			end;

	end = pPath.find_last_of('.');
	if (end == std::string::npos)
		return pPath;

	end = pPath.find_first_of('/', end);
	if (end == std::string::npos)
		return pPath;

	result = pPath.substr(0, end);
	if (result.empty())
		return "";

	return result;
}

// -----------------------------------------------------------------------------
// subInfo
// -----------------------------------------------------------------------------
std::string	subInfo(const std::string& pPath)
{
	std::string		result;
	size_t			start;

	start = pPath.find_last_of('.');
	if (start == std::string::npos)
		return "";

	start = pPath.find_first_of('/', start);
	if (start == std::string::npos)
		return "";

	result = pPath.substr(start, pPath.length() - start);
	if (result.empty())
		return "";

	return result;
}

} // namespace path