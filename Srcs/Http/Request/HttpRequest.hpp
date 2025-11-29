/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:14 by achu              #+#    #+#             */
/*   Updated: 2025/11/29 16:50:50 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_REQUEST_HPP__
#define __HTTP_REQUEST_HPP__

#include "../HttpConnection.hpp"
#include <string>
#include <vector>
#include <exception>

#define MAX_METHOD_LENGTH		8
#define MAX_URI_LENGTH			2048
#define MAX_HEADER_LENGTH		2048
#define MAX_BODY_LENGTH			1048576

enum	e_request_state
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
	, BODY_CHUNKED_SIZE
	, BODY_CHUNKED_SIZE_ALMOST_DONE
	, BODY_CHUNKED_SIZE_DONE
	, BODY_CHUNKED_DATA
	, BODY_CHUNKED_DATA_ALMOST_DONE
	, BODY_CHUNKED_DATA_DONE
	, BODY_CHUNKED_ALMOST_DONE
	, BODY_CHUNKED_DONE
	, BODY_CONTENT
	, PARSING_DONE
};

class HttpRequest {

private:

	e_request_state		_state;
	std::string			_buffer;

	http::e_method		_method;
	std::string			_path;
	std::string			_query;
	std::string			_fragment;
	std::string			_verMaj;
	std::string			_verMin;

	std::vector<std::pair<std::string, std::string> >		_headers;

	std::string			_body;
	std::string			_transferEncoding;
	int					_transferLength;
	int					_contentLength;

	http::e_status_code										_status;

private:

	void		addHeader(const std::string& pKey, const std::string& pValue);
	void		setLastHeader(const std::string& pValue);

public:

	HttpRequest(void);	
	~HttpRequest(void);

	void	feed(char *pBuffer, size_t pSize);

	http::e_method		getMethod(void) const		{ return (_method);   };
	std::string			getPath(void) const  		{ return (_path);     };
	std::string			getQuery(void) const		{ return (_query);    };
	std::string			getFragment(void) const		{ return (_fragment); };
	std::string			getVerMaj(void) const		{ return (_verMaj);   };
	std::string			getVerMin(void) const		{ return (_verMin);   };

	std::vector<std::pair<std::string, std::string> >		getHeaders(void) const { return (_headers); };

	bool				hasHeaderName(const std::string& pKey) const;
	std::string			getHeaderValue(const std::string& pKey) const;
	std::string			getHeaderValueAt(int pIdx) const;

	std::string					getBody(void) const			{ return (_body);   };
	http::e_status_code			getStatusCode(void) const	{ return (_status); };

	class BadRequestException : public std::exception {
	public:
		const char*	what() const throw();
	};

	class LengthRequiredException : public std::exception {
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

	class  ContentTooLargeException : public std::exception {
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
