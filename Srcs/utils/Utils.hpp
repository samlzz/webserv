/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 17:48:56 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/08 17:50:08 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <string>
# include <vector>

namespace utils
{

std::vector<std::string>	split(const std::string& pStr, const std::string& pDelims);

std::string					trim(const std::string& pStr, const std::string& pDelims);

}

#endif /* __UTILS_HPP__ */
