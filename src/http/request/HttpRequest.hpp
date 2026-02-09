/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:14 by achu              #+#    #+#             */
/*   Updated: 2026/01/29 18:02:33 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_REQUEST_HPP__
#define __HTTP_REQUEST_HPP__

#include "Cookies.hpp"
#include "IHttpRequest.hpp"

#include <string>
#include <vector>

#include "http/HttpTypes.hpp"
#include "http/response/BuffStream.hpp"

#define MAX_METHOD_LENGTH		8
#define MAX_URI_LENGTH			2 * 1024
#define MAX_HEADER_LENGTH		2 * 1024
#define CLIENT_MAX_BODY_SIZE	1 * 1024 * 1024

#define REQ_TIMEOUT_HEADER		10000   // 10 s
#define REQ_TIMEOUT_BODY		120000 // 120 s

enum	e_request_state
	{ LINE_METHOD
	, LINE_SPACE_BEFORE_URI
	, LINE_URI_SLASH
	, LINE_URI_PATH
	, LINE_URI_QUERY
	, LINE_URI_FRAGMENT
	, LINE_SPACE_BEFORE_VER
	, LINE_HTTP_H
	, LINE_HTTP_HT
	, LINE_HTTP_HTT
	, LINE_HTTP_HTTP
	, LINE_HTTP_SLASH
	, LINE_HTTP_MAJOR_VER
	, LINE_HTTP_DOT
	, LINE_HTTP_MINOR_VER
	, LINE_ALMOST_DONE
	, LINE_DONE
	, HEADER_START
	, HEADER_FIELD_START
	, HEADER_FIELD
	, HEADER_COLON
	, HEADER_VALUE_START
	, HEADER_VALUE
	, HEADER_VALUE_ALMOST_DONE
	, HEADER_VALUE_DONE
	, HEADER_END
	, BODY_START
	, BODY_TRANSFER_HEXA
	, BODY_TRANSFER_HEXA_ALMOST_DONE
	, BODY_TRANSFER_HEXA_DONE
	, BODY_TRANSFER_DATA
	, BODY_TRANSFER_DATA_ALMOST_DONE
	, BODY_TRANSFER_DATA_DONE
	, BODY_TRANSFER_END
	, BODY_CONTENT
	, PARSING_ERROR
	, PARSING_DONE
};

class HttpRequest : public IHttpRequest {
private:
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

		http::t_headers		headers;
		std::vector<char>	body;
	};

	mutable Cookies			_cookies;
	Request					_request;
	e_request_state			_state;
	time_t					_tsStart;
	http::e_status_code		_code;

	// ========= Buffer =========
	std::string			_buffer;
	std::string			_temp;

	// ====== Body Consume ======
	size_t				_transferLength;
	size_t				_contentLength;

	void	setError(const http::e_status_code pCode);

public:
	HttpRequest(void);
	~HttpRequest(void);

	// ======== Lifecycle ========
	virtual void		feed(char *pBuffer, size_t pSize);
	virtual void		reset();

	// ====== Request state ======
	virtual bool		isDone() const;
	virtual bool		isError() const;

	// ===== Getter / Setter =====
	http::e_method			getMethod() const;
	const std::string		&getPath() const;
	const std::string		&getQuery() const;
	const std::string		&getFragment() const;
	int						getVerMaj() const;
	int						getVerMin() const;
	const http::t_headers	&getHeaders() const;
	const t_bytes			&getBody() const;
	http::e_status_code		getStatusCode() const;
	Cookies					&getCookies() const;

	// ======= Header Utils =======
	void				setField(const std::string& pKey, const std::string& pValue);
	bool				hasField(const std::string& pKey) const;
	std::string			getField(const std::string& pKey) const;

	void				checkTimeout(time_t now);
};

std::ostream&		operator<<(std::ostream& pOut, const HttpRequest& pRequest);

#endif
