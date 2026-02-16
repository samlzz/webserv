/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileSystemUtils.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 14:30:19 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/10 18:55:03 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FILE_SYSTEM_UTILS_HPP__
# define __FILE_SYSTEM_UTILS_HPP__

# include <string>
# include <stdint.h>
# include <unistd.h>
# include <sys/stat.h>

namespace fs
{

bool		isExist(const std::string &path, struct stat *st = NULL);

bool		isFile(const std::string &path);
bool		isFile(struct stat st);

bool		isDir(const std::string &path);
bool		isDir(struct stat st);

int			openReadOnly(const std::string &path);

ssize_t		size(const std::string &path);

std::string	url_decode(const std::string &str);

} // namespace fs

#endif /* __FILESYSTEMUTILS_HPP__ */
