/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trim.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 01:05:25 by achu              #+#    #+#             */
/*   Updated: 2025/12/08 18:26:53 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <string>

std::string		trim(const std::string& pStr, const std::string& pDelims)
{
	size_t	start = pStr.find_first_not_of(pDelims);
	if (start == std::string::npos)
		return ("");

	size_t	end = pStr.find_last_not_of(pDelims);
	return (pStr.substr(start, end - start + 1));
}