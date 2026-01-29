/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 02:35:19 by achu              #+#    #+#             */
/*   Updated: 2026/01/29 03:01:04 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRING_HPP__
#define __STRING_HPP__

#include <string>

namespace utils
{

template <class T>
std::string		toString(T pClass)
{
	std::ostringstream out;
	out << pClass;
	return out.str();
}

std::string		toUpper(const std::string& pStr)
{
	std::string upper;

	for (size_t i = 0; i < pStr.length(); i++)
		upper.push_back(std::toupper(static_cast<unsigned char>pStr[i]));

	return upper;
}

} // namespace utils


#endif