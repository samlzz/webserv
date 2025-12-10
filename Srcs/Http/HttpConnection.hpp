/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConnection.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 22:31:56 by achu              #+#    #+#             */
/*   Updated: 2025/12/10 15:40:58 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_CONNECTION_HPP__
#define __HTTP_CONNECTION_HPP__

#include "Request/HttpRequest.hpp"
#include "Response/HttpResponse.hpp"

namespace http
{
	enum	e_method
		{ MTH_GET
		, MTH_HEAD
		, MTH_POST
		, MTH_PUT		
		, MTH_DELETE
		, MTH_UNKNOWN
	};

	enum	e_status_code
		{ SC_OK = 200
		, SC_CREATED = 201
		, SC_NO_CONTENT = 204
		, SC_MOVED_PERMANENTLY = 301
		, SC_FOUND = 302
		, SC_BAD_REQUEST = 400
		, SC_FORBIDDEN = 403
		, SC_NOT_FOUND = 404
		, SC_METHOD_NOT_ALLOWED = 405
		, SC_LENGTH_REQUIRED = 411
		, SC_CONTENT_TOO_LARGE = 413
		, SC_URI_TOO_LONG = 414
		, SC_INTERNAL_SERVER_ERROR = 500
		, SC_NOT_IMPLEMENTED = 501
	};
};

#endif