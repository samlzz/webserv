/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trim.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 01:05:25 by achu              #+#    #+#             */
/*   Updated: 2025/10/29 15:03:37 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

std::string		Trim(const std::string& pStr, const std::string& pDelims)
{
	size_t	start = pStr.find_first_not_of(pDelims);
	if (start == std::string::npos)
		return ("");

	size_t	end = pStr.find_last_not_of(pDelims);
	return (pStr.substr(start, end - start + 1));
}