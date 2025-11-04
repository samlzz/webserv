/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:03 by achu              #+#    #+#             */
/*   Updated: 2025/11/04 02:56:24 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include "HttpSession.h"
#include <string>
#include <vector>
#include <exception>

class HttpResponse {

private:

	std::string		_version;
	std::string		_statusCode;
	std::string		_statusMessage;

	std::string		_allow;
	std::string		_cacheControl;
	std::string		_connection;
	std::string		_contentEncoding;
	std::string		_contentType;
	std::string		_contentLength;
	std::string		_contentLocation;
	std::string		_date;
	std::string		_lastModified;
	std::string		_location;
	std::string		_retryAfter;
	std::string		_server;
	std::string		_setCookie;
	std::string		_tranferEncoding;
	std::string		_vary;

	std::string		_body;

public:
	HttpResponse(void);
	~HttpResponse(void);
};

#endif