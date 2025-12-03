/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:03 by achu              #+#    #+#             */
/*   Updated: 2025/11/24 16:41:54 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "../HttpConnection.hpp"
#include <string>
#include <vector>
#include <exception>

enum	e_response_state
	{ RES_SEND_HEADER
	, RES_SEND_BODY
};

class HttpResponse {

private:

	e_response_state		_state;

	std::string		_responseLine;
	std::string		_headers;
	std::string		_body;

	http::e_method	_method;
	std::string		_path;
	std::string		_verMaj;
	std::string		_verMin;
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

	void		buildResponseLine(void);
	void		buildHeaders(void);
	void		buildBody(void);
	
public:

	HttpResponse(void);
	~HttpResponse(void);

	void		build(const http::e_method& pMethod, const std::string& pPath);

	std::string		getVersion(void) const;
	std::string		getStatusCode(void) const;
	std::string		getStatusMessage(void) const;

};

#endif