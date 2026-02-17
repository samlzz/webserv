/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AddrInfo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 17:08:15 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 16:34:55 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "AddrInfo.hpp"

// ============================================================================
// Construction 
// ============================================================================

AddrInfo::AddrInfo(): addr(), port(0)
{}

AddrInfo::AddrInfo(const std::string &a, uint16_t p): addr(a), port(p)
{}

AddrInfo::AddrInfo(const struct sockaddr_storage &address): addr(), port(0)
{
	char		ip[INET6_ADDRSTRLEN];
	const char	*ntopResult = NULL;

	ip[0] = '\0';

	if (address.ss_family == AF_INET)
	{
		const struct sockaddr_in* a =
			reinterpret_cast<const struct sockaddr_in*>(&address);
		ntopResult = inet_ntop(AF_INET, &a->sin_addr, ip, sizeof(ip));
		port = ntohs(a->sin_port);
	}
	else if (address.ss_family == AF_INET6)
	{
		const struct sockaddr_in6* a =
			reinterpret_cast<const struct sockaddr_in6*>(&address);
		ntopResult = inet_ntop(AF_INET6, &a->sin6_addr, ip, sizeof(ip));
		port = ntohs(a->sin6_port);
	}

	if (ntopResult)
		addr = std::string(ip);
	else
		addr.clear();
}

std::ostream	&operator<<(std::ostream &os, const AddrInfo &addr)
{
	os << addr.addr << ':' << addr.port;
	return os;
}