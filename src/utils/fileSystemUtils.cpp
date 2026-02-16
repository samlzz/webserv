/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileSystemUtils.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 14:41:48 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/10 18:54:00 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sstream>

#include "fileSystemUtils.hpp"

namespace fs
{

// -----------------------------------------------------------------------------
// isExist
// -----------------------------------------------------------------------------

bool isExist(const std::string &path, struct stat *st)
{
	struct stat st__;

	if (stat(path.c_str(), st ? st : &st__) != 0)
		return false;
	return true;
}

// -----------------------------------------------------------------------------
// isFile
// -----------------------------------------------------------------------------
bool isFile(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return false;
	return S_ISREG(st.st_mode) && (st.st_mode & S_IRUSR);
}

bool isFile(struct stat st)
{
	return S_ISREG(st.st_mode);
}

// -----------------------------------------------------------------------------
// isDir
// -----------------------------------------------------------------------------
bool isDir(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
}

bool isDir(struct stat st)
{
	return S_ISDIR(st.st_mode);
}

// -----------------------------------------------------------------------------
// openReadOnly
// -----------------------------------------------------------------------------
int openReadOnly(const std::string &path)
{
	int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	return fd;
}

// -----------------------------------------------------------------------------
// size
// -----------------------------------------------------------------------------
ssize_t size(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return -1;

	return static_cast<ssize_t>(st.st_size);
}

/// -----------------------------------------------------------------------------
// decode hexa
/// -----------------------------------------------------------------------------
//Decode URL encoded string (%XX -> character)
std::string url_decode(const std::string &str)
{
	std::string result;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '%' && i + 2 < str.length())
		{
			int hex_val;
			std::istringstream hex_stream(str.substr(i + 1, 2));
			if (hex_stream >> std::hex >> hex_val)
			{
				result += static_cast<char>(hex_val);
				i += 2;
			}
			else
				result += str[i];
		}
		else if (str[i] == '+')
			result += ' ';
		else
			result += str[i];
	}
	return result;
}

} // namespace fs

