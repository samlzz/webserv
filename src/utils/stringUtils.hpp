/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 02:35:19 by achu              #+#    #+#             */
/*   Updated: 2026/02/20 15:51:34 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRING_UTILS_HPP__
# define __STRING_UTILS_HPP__

# include <algorithm>
# include <sstream>
# include <string>

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

inline std::string trim(const std::string& s)
{
	size_t start = 0;
	while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
		start++;

	size_t end = s.size();
	while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'))
		end--;

	return s.substr(start, end - start);
}

namespace {

char	_toLowerChar(char c)
{
	return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}
}

inline std::string	&lowerCase(std::string &s)
{
	std::transform(
		s.begin(), s.end(), s.begin(),
		_toLowerChar);
	return s;
}

} // namespace utils


#endif