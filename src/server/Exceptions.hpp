/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:25:07 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/19 11:24:14 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EXCEPTIONS_HPP__
# define __EXCEPTIONS_HPP__

# include <cerrno>
# include <cstring>
# include <stdexcept>
# include <stdint.h>

class SysError: public std::runtime_error {

private:
	int	_errno;

public:

	SysError(const std::string &msg)
		: std::runtime_error(
			"webserv: " + msg + ": " + strerror(errno)
		)
		, _errno(errno)
	{}

	int	code(void) const	{ return _errno; }
};

class LogicError : public std::runtime_error {
public:
	LogicError(const std::string& msg)
		: std::runtime_error("webserv: " + msg) {}
};


#endif /* __EXCEPTIONS_HPP__ */