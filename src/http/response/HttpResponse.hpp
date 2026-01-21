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

#include "http/response/IHttpResponse.hpp"
#include "http/response/ChunkedStream.hpp"
#include "http/request/HttpRequest.hpp"

#include "server/connections/ConnEvent.hpp"
#include "config/Config.hpp"

# ifndef __MAX_CHUNK__
# define __MAX_CHUNK__	3
# endif

# ifndef __SIZE_OF_CHUNK__
# define __SIZE_OF_CHUNK__	32000
# endif

class HttpResponse : public IHttpResponse {
public:

	HttpResponse(const Config::Server& pServer);
	~HttpResponse(void);

	// ========== Life Cycle ==========
	virtual ConnEvent			build(const HttpRequest& pRequest, IWritableNotifier &notifier);
	void						chunkStream(void);
	virtual void				reset(void);

	// ========== Output Production ==========
	virtual IChunkedStream&		stream(void);
	virtual bool				isDone() const;
	virtual bool				isConnectionClose(void) const;

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
	HttpRequest							_request;
	const Config::Server				&_server;
	const Config::Server::Location		*_location;

	Response		_response;

	bool				_isDone;
	bool				_isConnection;
	ChunkedStream		_chunkedStream;

	int			fd;

	void		setError(int pCode);

	void		handleGET(void);
	void		handleHEAD(void);
	void		handlePOST(void);
	void		handlePUT(void);
	void		handleDELETE(void);

	void		loadFile(const std::string& pPath);

	std::string		toString(void) const;
};

#endif