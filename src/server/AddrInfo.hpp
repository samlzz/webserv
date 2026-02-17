/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AddrInfo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 17:04:42 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 16:34:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ADDRINFO_HPP__
# define __ADDRINFO_HPP__

#include <ostream>
# include <stdint.h>
# include <string>
# include <sys/socket.h>

struct AddrInfo
{
	std::string	addr;
	uint16_t	port;

	AddrInfo();
	AddrInfo(const std::string& a, uint16_t p);
	AddrInfo(const struct sockaddr_storage &address);
};

std::ostream	&operator<<(std::ostream &os, const AddrInfo &addr);

#endif /* __ADDRINFO_HPP__ */