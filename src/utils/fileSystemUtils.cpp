/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileSystemUtils.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 14:41:48 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 15:04:55 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

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

} // namespace fs

