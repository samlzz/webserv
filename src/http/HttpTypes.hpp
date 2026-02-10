/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpTypes.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 22:31:56 by achu              #+#    #+#             */
/*   Updated: 2026/01/27 10:46:16 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_TYPES_HPP__
#define __HTTP_TYPES_HPP__

# include <map>
# include <string>
# include <ostream>

namespace http
{

	enum	e_method
	{
		MTH_UNKNOWN = 0
		, MTH_GET
		, MTH_HEAD
		, MTH_POST
		, MTH_PUT
		, MTH_DELETE
	};
	std::ostream	&operator<<(std::ostream &os, e_method method);

	enum	e_status_code
	{
		SC_NONE = 0
		, SC_OK = 200
		, SC_CREATED = 201
		, SC_NO_CONTENT = 204

		, SC_MOVED_PERMANENTLY = 301
		, SC_FOUND = 302

		, SC_BAD_REQUEST = 400
		, SC_FORBIDDEN = 403
		, SC_NOT_FOUND = 404
		, SC_METHOD_NOT_ALLOWED = 405
		, SC_REQUEST_TIMEOUT = 408
		, SC_CONFLICT = 409
		, SC_LENGTH_REQUIRED = 411
		, SC_CONTENT_TOO_LARGE = 413
		, SC_URI_TOO_LONG = 414
		, SC_UNSUPPORTED_MEDIA_TYPE = 415
		, SC_HEADER_FIELDS_TOO_LARGE = 431

		, SC_INTERNAL_SERVER_ERROR = 500
		, SC_NOT_IMPLEMENTED = 501
		, SC_BAD_GATEWAY = 502
		, SC_GATEWAY_TIMEOUT = 504
		, SC_VERSION_NOT_SUPPORTED = 505
		, SC_INSUFFICIENT_STORAGE = 507
	};
	std::ostream	&operator<<(std::ostream &os, e_status_code code);

	enum e_body_kind
	{
		CT_UNKNOWN = 0
		, CT_APPLICATION_X_WWW_FORM_URLENCODED
		, CT_MULTIPART_FORM_DATA
		, CT_BINARY
		, CT_TEXT_PLAIN
	};

	typedef std::map<std::string, std::string>	t_headers;
	typedef std::map<std::string, std::string>	t_cookies;

} // namespace http

#endif /* __HTTP_TYPES_HPP__ */