/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:03 by achu              #+#    #+#             */
/*   Updated: 2026/01/12 00:57:40 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include "../HttpStatus.hpp"
#include "../Request/HttpRequest.hpp"
#include "../CGI/CgiHandler.hpp"
#include <string>
#include <vector>
#include <exception>

enum	e_response_state
	{ RES_START
	, RES_END
};

class HttpResponse {

private:

	HttpRequest		_request;
	CgiHandler		_cgiHandler;

	std::string		_responseLine;
	std::string		_statusCode;
	std::string		_statusMessage;
	std::string		_body;

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

	void		loadFile(const std::string& pPath, int code);

	bool		isCGI(void);

	void		handleGET(void);
	void		handleHEAD(void);
	void		handlePOST(void);
	void		handlePUT(void);
	void		handleDELETE(void);
	void		handleERROR(int pCode);
	
public:

	HttpResponse(HttpRequest pRequest);
	~HttpResponse(void);

	void				build();
	std::string			toString(void);

	std::string		getVersion(void) const;
	std::string		getStatusCode(void) const;
	std::string		getStatusMessage(void) const;
};

#endif