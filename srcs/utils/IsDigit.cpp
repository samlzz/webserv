/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IsDigit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:38:33 by achu              #+#    #+#             */
/*   Updated: 2025/11/02 00:44:15 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

bool	isDigit(const std::string& pStr)
{
	if (pStr.empty())
		return (false);

	size_t	pos = (pStr[0] == '-' || pStr[0] == '+') ? 1 : 0;

	return (pStr.find_first_not_of("0123456789", pos) == std::string::npos);
}