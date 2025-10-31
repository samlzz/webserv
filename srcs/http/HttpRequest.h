/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:14 by achu              #+#    #+#             */
/*   Updated: 2025/10/29 18:43:25 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include "HttpCommon.h"
#include <string>
#include <vector>
#include <exception>

#define MAX_METHOD_LENGTH		8
#define MAX_URI_LENGTH			4096
#define MAX_HEADER_LENGTH			2048

enum	e_parse_state
	{ REQ_METHOD
	, REQ_SPACE_BEFORE_URI
	, REQ_URI_SLASH
	, REQ_URI_PATH
	, REQ_URI_QUERY
	, REQ_URI_FRAGMENT
	, REQ_SPACE_BEFORE_VER
	, REQ_HTTP_H
	, REQ_HTTP_HT
	, REQ_HTTP_HTT
	, REQ_HTTP_HTTP
	, REQ_HTTP_SLASH
	, REQ_HTTP_MAJOR_VER
	, REQ_HTTP_DOT
	, REQ_HTTP_MINOR_VER
	, REQ_ALMOST_DONE
	, REQ_DONE
	, HEADER_START
	, HEADER_FIELD_NAME_START
	, HEADER_FIELD_NAME
	, HEADER_FIELD_COLON
	, HEADER_FIELD_VALUE_START
	, HEADER_FIELD_VALUE
	, HEADER_FIELD_ALMOST_DONE
	, HEADER_FIELD_DONE
	, HEADER_END
	, BODY_MESSAGE_START
	, BODY_MESSAGE
	, PARSING_ALMOST_DONE
	, PARSING_DONE
};

class HttpRequest {

private:

	std::string			_buffer;
	e_parse_state		_state;

	http::e_method		_method;
	std::string			_path;
	std::string			_query;
	std::string			_fragment;
	std::string			_version;

	std::vector<std::pair<std::string, std::string>>		_headers;

	std::string			_body;
	bool				_transferEncoding;
	size_t				_contentLength;

	http::e_status_code										_status;

public:

	HttpRequest(void);	
	~HttpRequest(void);

	void	feed(char *pBuffer, size_t pSize);

	http::e_method											getMethod(void) const;
	std::string												getPath(void) const;
	std::string												getQuery(void) const;
	std::string												getFragment(void) const;
	std::string												getVersion(void) const;
	std::vector<std::pair<std::string, std::string>>		getHeaders(void) const;
	std::string												getHeader(const std::string& pKey) const;
	std::string												getHeaderAt(int pIdx) const;
	std::string												getBody(void) const;
	http::e_status_code										getStatusCode(void) const;

	void			setMethod(const http::e_method& pMethod);
	void			setPath(const std::string& pPath);
	void			setQuery(const std::string& pQuery);
	void			setFragment(const std::string& pFragment);
	void			setVersion(const std::string& pVersion);
	void			addHeader(const std::string& pKey, const std::string& pValue);
	void			setLastHeader(const std::string& pValue);
	void			setBody(const std::string& pBody);
	void			setStatusCode(const http::e_status_code& pCode);

	class BadRequestException : public std::exception {
	public:
		const char*	what() const throw();
	};

	class URITooLongException : public std::exception {
	public:
		const char*	what() const throw();
	};

	class HeaderFieldsTooLargeException : public std::exception {
	public:
		const char*	what() const throw();
	};

	class NotImplementedException : public std::exception {
	public:
		const char*	what() const throw();
	};
};

std::ostream&		operator<<(std::ostream& pOut, const HttpRequest& pRequest);

#endif
