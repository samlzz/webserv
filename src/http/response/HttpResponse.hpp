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

#include "BuffStream.hpp"
#include "interfaces/IFifoStream.hpp"
#include "interfaces/IHttpResponse.hpp"
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

private:

	// ========== Response Reply ==========
	struct Response
	{
		struct StatusCode {
			int				code;
			std::string		reason;
		};
		void	setStatusCode(const int &pCode);

		StatusCode			statusCode;
		http::t_headers		headers;
		std::string			body;
	};

private:
	HttpRequest							_request;
	const Config::Server				&_server;
	const Config::Server::Location		*_location;

	Response							_response;

	bool								_isDone;
	bool								_isConnection;
	BuffStream							_outputStream;
	int									_fd;

	// ========== Helpers methods ==========
	void		setError(int pCode);
	void		loadFile(const std::string& pPath);
	void		addHeader(const std::string &pHeader, const std::string &pContent);
	bool		validUploadPath(std::string& path);
	std::string buildUploadPath(std::string &filename,http::e_body_kind expression);
	// ========== ContentType handler (for POST/PUT) ==========
	void		handleMultipart(http::e_method curMethod);
	void		handleUrlEncoded(http::e_method curMethod);
	void		handleOctetStream(http::e_method curMethod);
	void		handleTextPlain(http::e_method curMethod);

	// ========== Methods handler ==========
	void		handleGET(void);
	void		handleHEAD(void);
	void		handlePOST(void);
	void		handlePUT(void);
	void		handleDELETE(void);

public:
	HttpResponse(const Config::Server& pServer);
	~HttpResponse(void);

	// ========== Life Cycle ==========
	virtual ConnEvent					build(const HttpRequest& pRequest, IWritableNotifier &notifier);
	void								fillStream(void);
	virtual void						reset(void);

	// ========== Output Production ==========
	virtual IFifoStreamView<t_bytes>&	stream(void);
	virtual bool						isDone() const;
	virtual bool						shouldCloseConnection(void) const;

friend std::ostream	&operator<<(std::ostream &pOut, const HttpResponse::Response &pResponse);
};

#endif