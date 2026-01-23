/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseUtils.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 09:21:48 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/23 20:42:26 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSE_UTILS_HPP__
# define __PARSE_UTILS_HPP__

# include <stdint.h>
# include <string>
# include <vector>

# include "http/HttpTypes.hpp"

namespace config_parse
{

// ========================================================================
// Generic parsing helpers
// ========================================================================

std::vector<std::string>		splitWords(const std::string &s);

uint64_t						parseUnsigned(const std::string &s, const std::string &what);

// ========================================================================
// Parsing helpers for each kind of data
// ========================================================================

bool							parseBool(const std::string &s);

http::e_method					parseMethod(const std::string &s);

size_t							parseSize(const std::string &s);

http::e_status_code				parseStatusCode(const std::string &s);

}

#endif /* __PARSE_UTILS_HPP__ */
