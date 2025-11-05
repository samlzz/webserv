/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:03 by achu              #+#    #+#             */
/*   Updated: 2025/11/05 17:44:33 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include "HttpSession.h"
#include <string>
#include <vector>
#include <exception>

enum	e_response_state
	{ RES_START
	, RES_METHOD
	, RES_BUILD_HEADER
	, RES_BUILD_BODY
	, RES_SEND_HEADER
	, RES_SEND_BODY
};

class HttpResponse {

private:

	e_response_state		_state;

	std::string		_responseLine;
	std::string		_headers;
	std::string		_body;

	std::string		_verMajor;
	std::string		_verMinor;
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

private:

	void		handleGET(void);
	void		handleHEAD(void);
	void		handlePOST(void);
	void		handlePUT(void);
	void		handleDELETE(void);
	
public:

	HttpResponse(void);
	~HttpResponse(void);

	void		build(const http::e_method& pMethod, const std::string& pPath);

	std::string		getVersion(void) const;
	std::string		getStatusCode(void) const;
	std::string		getStatusMessage(void) const;

	
};

#endif