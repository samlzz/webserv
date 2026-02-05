/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 12:21:10 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/31 12:23:30 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PATHUTILS_HPP__
# define __PATHUTILS_HPP__

# include <string>

namespace path
{

std::string	subExt(const std::string &filePath);

std::string	subPath(const std::string& pPath);

std::string	subInfo(const std::string& pPath);

} // namespace path

#endif /* __PATHUTILS_HPP__ */
