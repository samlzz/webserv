/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:25:07 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/19 13:17:15 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EXCEPTIONS_HPP__
# define __EXCEPTIONS_HPP__

# include <cerrno>
# include <cstring>
# include <stdexcept>
# include <stdint.h>

class WsServerError: public std::runtime_error {
public:
	WsServerError(const std::string &msg): std::runtime_error("webserv: " + msg) {}
};

class SysError: public WsServerError {

private:
	int	_errno;

public:

	SysError(const std::string &msg)
		: WsServerError(msg + ": " + strerror(errno))
		, _errno(errno)
	{}

	int	code(void) const	{ return _errno; }
};

#endif /* __EXCEPTIONS_HPP__ */