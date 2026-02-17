/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convertUtils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:55:23 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 14:57:44 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CONVERT_UTILS_HPP__
# define __CONVERT_UTILS_HPP__

# include <string>
# include <stdint.h>

namespace convert {

// ============================================================================
// Inlines functions
// ============================================================================

static inline bool	isDec(const std::string& pStr)
{
	if (pStr.empty())
		return (false);

	return (pStr.find_first_not_of("0123456789") == std::string::npos);
}

static inline bool	isHex(const std::string& pStr)
{
	if (pStr.empty())
		return (false);

	return (pStr.find_first_not_of("0123456789ABCDEFabcdef") == std::string::npos);
}

// ============================================================================
// Convertion functions
// ============================================================================

// Hexadecimal to Decimal
int32_t		htod(const std::string& pHex);

// Decimal to Hexadecimal
std::string	dtoh(int32_t pDec);

} // namespace convert

#endif /* __CONVERT_UTILS_HPP__ */
