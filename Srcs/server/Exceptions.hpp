/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:25:07 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/29 09:29:41 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EXCEPTIONS_HPP__
# define __EXCEPTIONS_HPP__

# include <cerrno>
# include <cstring>
# include <stdexcept>
# include <stdint.h>

class SysError: public std::runtime_error {
public:
	SysError(const std::string &msg)
		: std::runtime_error("webserv: " + msg + ": " + strerror(errno)) {}
	SysError(const std::string &msg, int32_t savedErrno)
		: std::runtime_error("webserv: " + msg + ": " + strerror(savedErrno)) {}
};

#endif /* __EXCEPTIONS_HPP__ */