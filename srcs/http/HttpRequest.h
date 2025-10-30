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
#include <unordered_map>
#include <string>

class HttpRequest {

private:

	http::e_method									_method;
	std::string										_path;
	std::string										_query;
	std::string										_version;
	std::unordered_map<std::string, std::string>	_headers;
	std::string										_body;

	enum	e_parse_state
		{ REQ_START
		, REQ_METHOD
		, REQ_SPACE_BEFORE_URL
		, REQ_URI_SLASH
		, REQ_URI_PATH
		, REQ_URI_QUERY
		, REQ_SPACE_BEFORE_VER
		, REQ_HTTP_H
		, REQ_HTTP_HT
		, REQ_HTTP_HTT
		, REQ_HTTP_HTTP
		, REQ_HTTP_SLASH
		, REQ_HTTP_MAJOR_VER
		, REQ_HTTP_DOT
		, REQ_HTTP_MINOR_VER
		, REQ_HTTP_END

		, HEADER_FIELD_NAME_START
		, HEADER_FIELD_NAME
		, HEADER_FIELD_NAME_END
		, HEADER_FIELD_VALUE_START
		, HEADER_FIELD_VALUE
		, HEADER_FIELD_VALUE_END

		, HEADER_ALMOST_DONE
		, HEADER_DONE

		, BODY_MESSAGE

		, PARSING_DONE
	};

public:

	HttpRequest(void);
	~HttpRequest(void);

	http::e_method		getMethod(void) const;
	std::string			getPath(void) const;
	std::string			getQuery(void) const;
	std::string			getVersion(void) const;
	std::string			getHeaders(const std::string& pKey) const;
	std::string			getBody(void) const;

	void			setMethod(const http::e_method& pMethod);
	void			setPath(const std::string& pPath);
	void			setQuery(const std::string& pQuery);
	void			setVersion(const std::string& pVersion);
	void			addHeaders(const std::string& pKey, const std::string& pValue);
	void			setBody(const std::string& pBody);
};

#endif
