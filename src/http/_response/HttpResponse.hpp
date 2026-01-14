  /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:03 by achu              #+#    #+#             */
/*   Updated: 2026/01/12 20:02:38 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include <string>

#include "IHttpResponse.hpp"
#include "http/request/HttpRequest.hpp"
#include "HttpStatus.hpp"
#include "Config.hpp"

/**
 * if (!_isCgi)
 * 		return _raw;
 * std::string chunk = getChunk(_cgiHandler.getOut());
 * if (_headSent)
 * 		return chunk;
 * else
 * {
 *     _headSent = true;
 *		return _raw + chunk;
 * }
 */

class HttpResponse : public IHttpResponse {
public:

	HttpResponse(const HttpRequest& pRequest, const Config::Server& pServer);
	~HttpResponse(void);

	// ========== State Control ==========
	void	build(void);
	bool	isDone() const;
	void	reset(void);
	bool	isConnectionClose(void) const;

	// ========== Output Production ==========
	bool					produceNext(void);
	const std::string&		buffer() const;

private:

	// ========== Response Reply ==========
	typedef std::map<std::string, std::string>	t_headers;
	struct Response
	{
		struct StatusCode {
			int				code;
			std::string		reason;
		};
		void	setStatusCode(const int &pCode);

		StatusCode			statusCode;
		t_headers			headers;
		std::string			body;
	};

	void	addHeader(const std::string &pHeader, const std::string &pContent);

	// ========== Response Config ==========
	const HttpRequest					&_request;
	const Config::Server				&_server;
	const Config::Server::Location		*_location;

	Response							_response;

	void		loadFile(const std::string& pPath, int code);

	void		handleGET(void);
	void		handleHEAD(void);
	void		handlePOST(void);
	void		handlePUT(void);
	void		handleDELETE(void);
	void		handleERROR(int pCode);
};

#endif