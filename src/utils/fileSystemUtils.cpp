/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileSystemUtils.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 14:41:48 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/31 12:21:41 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <stdint.h>

#include "fileSystemUtils.hpp"

namespace fs
{

// -----------------------------------------------------------------------------
// checkPerms
// -----------------------------------------------------------------------------
bool checkPerms(const std::string &path, int32_t perms)
{
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return false;

	if ((perms & P_EXIST) && errno == ENOENT)
		return false;

	if (perms & P_READ)
	{
		if (!(st.st_mode & S_IRUSR))
			return false;
	}

	if (perms & P_WRITE)
	{
		if (!(st.st_mode & S_IWUSR))
			return false;
	}

	if (perms & P_EXEC)
	{
		if (!(st.st_mode & S_IXUSR))
			return false;
	}

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

} // namespace fs

