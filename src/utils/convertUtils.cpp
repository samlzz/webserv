/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convertUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:55:38 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 15:00:24 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "convertUtils.hpp"
#include <sstream>
#include <string>

namespace convert {

std::string	dtoh(int32_t pDec)
{
	std::stringstream	out;

	out << std::hex << pDec;
	if (out.fail())
		return "";
	return out.str();
}

int32_t	htod(const std::string& pHex)
{
	if (pHex.empty())
		return -1;

	int32_t	result = 0;

	for (size_t i = 0; i < pHex.size(); ++i)
	{
		char c = pHex[i];
		result = result * 16;

		if (c >= '0' && c <= '9') result += c - '0';
		else if (c >= 'A' && c <= 'F') result += c - 'A' + 10;
		else if (c >= 'a' && c <= 'f') result += c - 'a' + 10;
		else return (-1);
	}
	return (result);
}

} // namespace convert
