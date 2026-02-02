/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileSystemUtils.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 14:30:19 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/31 12:21:32 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FILE_SYSTEM_UTILS_HPP__
# define __FILE_SYSTEM_UTILS_HPP__

# include <string>
# include <stdint.h>
# include <unistd.h>

namespace fs
{

enum e_perm
{
	P_EXIST = 1 << 0,
	P_READ = 1 << 1,
	P_WRITE = 1 << 2,
	P_EXEC = 1 << 3
};

bool		checkPerms(const std::string &path, int32_t perms);
bool		isFile(const std::string &path);
bool		isDir(const std::string &path);

int			openReadOnly(const std::string &path);

ssize_t		size(const std::string &path);

} // namespace fs

#endif /* __FILESYSTEMUTILS_HPP__ */
