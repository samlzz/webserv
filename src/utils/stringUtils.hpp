/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 02:35:19 by achu              #+#    #+#             */
/*   Updated: 2026/01/29 16:09:33 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRING_UTILS_HPP__
#define __STRING_UTILS_HPP__

#include <sstream>
#include <string>

namespace str
{

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

} // namespace utils


#endif