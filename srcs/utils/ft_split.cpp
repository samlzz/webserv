/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 21:57:29 by achu              #+#    #+#             */
/*   Updated: 2025/10/29 01:51:48 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

std::vector<std::string>	ft_split(const std::string& pStr, const std::string& pDelims)
{
	std::vector<std::string>	tokens;

	size_t		start = 0;

	while (start < pStr.length())
	{
		size_t	token_begin = pStr.find_first_not_of(pDelims, start);
		if (token_begin == std::string::npos)
			break;

		size_t	token_end = pStr.find_first_of(pDelims, token_begin);

		tokens.push_back(pStr.substr(token_begin, token_end - token_begin));

		if (token_end == std::string::npos)
			break;

		start = token_end + 1;
	}
	return (tokens);
}