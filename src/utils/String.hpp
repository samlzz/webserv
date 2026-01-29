/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 02:35:19 by achu              #+#    #+#             */
/*   Updated: 2026/01/29 16:08:03 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRING_HPP__
#define __STRING_HPP__

#include <sstream>
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

} // namespace utils


#endif