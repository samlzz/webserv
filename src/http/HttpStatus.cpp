/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 22:43:24 by achu              #+#    #+#             */
/*   Updated: 2026/01/13 22:07:46 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpStatus.hpp"

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

StatusCode		setStatusCode(int code)
{
	static const StatusCode statusData[] = {
		{ 200, "OK" },
		{ 201, "Created" },
		{ 204, "No Content" },
		{ 301, "Moved Permanently" },
		{ 400, "Bad Request" },
		{ 403, "Forbidden" },
		{ 404, "Not Found" },
		{ 500, "Internal Server Error" },
		{ 501, "Not Implemented" },
		{ 0,   NULL }
	};

	for (int i = 0; statusData[i].code != 0; i++) {
		if (statusData[i].code == code)
			return (statusData[i]);
	}

	return (setStatusCode(SC_INTERNAL_SERVER_ERROR));
}

} // namespace http