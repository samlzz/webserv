/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   urlUtils.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:54:32 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 15:26:35 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __URLUTILS_HPP__
# define __URLUTILS_HPP__

# include <string>

namespace url {

// ============================================================================
// Inline functions
// ============================================================================

static inline bool	isExternal(const std::string &p)
{
	return (p.compare(0, 7, "http://") == 0 ||
			p.compare(0, 8, "https://") == 0);
}

static inline bool	isInternal(const std::string &p)
{
	return (p.size() > 0 && p[0] == '/');
}

// ============================================================================
// Decode functions
// ============================================================================

std::string	decode(const std::string &encoded);
bool		isValidEncoded(const std::string &encoded);

} // namespace url

#endif /* __URLUTILS_HPP__ */
