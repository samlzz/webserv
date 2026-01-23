/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:14 by achu              #+#    #+#             */
/*   Updated: 2026/01/23 02:02:00 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_REQUEST_HPP__
#define __HTTP_REQUEST_HPP__

#include <map>
#include <string>

#include "http/request/IHttpRequest.hpp"
#include "http/HttpStatus.hpp"

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
	, PARSING_ERROR
};

class HttpRequest : public IHttpRequest {
private:
	typedef std::map<std::string, std::string>	t_headers;
	struct Request {
		// ====== Response Line ======
		http::e_method	method;
		struct Uri {
			std::string	path;
			std::string	query;
			std::string	fragment;
		};
		Uri		uri;
		int		verMaj;
		int		verMin;

		t_headers	headers;
		std::string	body;
	};
	
	e_request_state		_state;
	std::string			_buffer;
	std::string			_valueBuf;

	Request		_request;

	std::string			_transferEncoding;
	int					_transferLength;
	int					_contentLength;

	http::e_status_code		_status;

	void	setError(const http::e_status_code pCode);

public:
	HttpRequest(void);	
	~HttpRequest(void);

	// ====== Lifecycle ======
	virtual void	feed(char *pBuffer, size_t pSize);
	virtual void	reset(void);

	// ====== Request state ======
	virtual bool	isDone(void) const;

	// ========== Getter / Setter ==========
	int	getState(void) const {return (_state); }
	http::e_method		getMethod(void) const		{ return (_request.method);       };
	std::string			getPath(void) const  		{ return (_request.uri.path);     };
	std::string			getQuery(void) const		{ return (_request.uri.query);    };
	std::string			getFragment(void) const		{ return (_request.uri.fragment); };
	int					getVerMaj(void) const		{ return (_request.verMaj);       };
	int					getVerMin(void) const		{ return (_request.verMin);       };

	t_headers			getHeaders(void) const { return (_request.headers); };
	void				addHeader(const std::string& pHeader, const std::string& pContent);
	bool				hasHeader(const std::string& pHeader) const;
	std::string			getHeader(const std::string& pHeader) const;

	std::string			getBody(void) const			{ return (_request.body); };

	int					getStatusCode(void) const	{ return (_status); };
};

std::ostream&		operator<<(std::ostream& pOut, const HttpRequest& pRequest);

#endif
