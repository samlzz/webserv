/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpTypes.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 03:42:39 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/29 06:26:14 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ostream>

#include "Http/HttpTypes.hpp"

namespace http
{

std::ostream	&operator<<(std::ostream &os, e_method method)
{
	switch (method)
	{
		case http::MTH_GET:     os << "GET"; break;
		case http::MTH_HEAD:    os << "HEAD"; break;
		case http::MTH_POST:    os << "POST"; break;
		case http::MTH_PUT:     os << "PUT"; break;
		case http::MTH_DELETE:  os << "DELETE"; break;
		default:                os << "UNKNOWN"; break;
	}
	return os;
}

std::ostream	&operator<<(std::ostream &os, e_status_code code)
{
	switch (code)
	{
		case http::SC_OK:                      os << "200"; break;
		case http::SC_CREATED:                 os << "201"; break;
		case http::SC_NO_CONTENT:              os << "204"; break;
		case http::SC_MOVED_PERMANENTLY:       os << "301"; break;
		case http::SC_FOUND:                   os << "302"; break;
		case http::SC_BAD_REQUEST:             os << "400"; break;
		case http::SC_FORBIDDEN:               os << "403"; break;
		case http::SC_NOT_FOUND:               os << "404"; break;
		case http::SC_METHOD_NOT_ALLOWED:      os << "405"; break;
		case http::SC_LENGTH_REQUIRED:         os << "411"; break;
		case http::SC_CONTENT_TOO_LARGE:       os << "413"; break;
		case http::SC_URI_TOO_LONG:            os << "414"; break;
		case http::SC_INTERNAL_SERVER_ERROR:   os << "500"; break;
		case http::SC_NOT_IMPLEMENTED:         os << "501"; break;
		default:                               os << "NONE"; break;
	}
	return os;
}

} // namespace http