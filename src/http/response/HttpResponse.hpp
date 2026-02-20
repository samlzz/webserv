/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 13:03:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/20 17:15:01 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

# include <ostream>
# include <stdint.h>

#include "config/Config.hpp"
# include "http/HttpTypes.hpp"
# include "http/request/HttpRequest.hpp"
# include "http/response/BuffStream.hpp"
# include "http/response/ResponsePlan.hpp"
# include "http/response/interfaces/IBodySource.hpp"
# include "http/response/interfaces/IMetaSource.hpp"
# include "http/transaction/HttpTransaction.hpp"

# define RESP_MAX_BUF_COUNT		8

# ifndef RESP_MAX_STREAM_SIZE
#  define RESP_BUFFER_SIZE		16 * 1024
# else
#  define RESP_BUFFER_SIZE RESP_MAX_STREAM_SIZE / RESP_MAX_BUF_COUNT
# endif

class HttpResponse {

private:
	class DecisionContext
	{
		const HttpRequest			&_request;
		const HttpTransaction		&_transaction;

	public:
		DecisionContext(const HttpRequest &req, const HttpTransaction &transac);

		const HttpRequest	&getReq(void) const;
		const Config::Server::Location	*
							getLocation(void) const;
		const ResponsePlan	getPlan(void) const;
	};

	DecisionContext			_ctx;
	http::e_status_code		_status;
	http::t_headers			_headers;
	IBodySource				*_body;
	BuffStream				_out;

	bool					_commited;
	bool					_done;

public:
	// ============================================================================
	// Construction / Destruction
	// ============================================================================
	HttpResponse(const ResponsePlan &plan,
					const HttpRequest &req,
					const HttpTransaction &transac);
	~HttpResponse();

	// ========================================================================
	// Accesors
	// ========================================================================

	bool						hasBody(void) const;
	IFifoStreamView<t_bytes>&	stream(void);
	bool						isDone() const;
	std::string					rawMeta(void) const;

	// ========================================================================
	// Life Cycle
	// ========================================================================

	bool						fillStream(void);
	
	bool						shouldCloseConnection(void) const;

private:
	// ========================================================================
	// Helpers
	// ========================================================================
	void						applyPlan(const ResponsePlan &plan);

	bool						fillMeta(IMetaSource *meta);
	bool						handleCgiRedirect(const std::string &redirectPath);

	// forbidden
	HttpResponse();
	HttpResponse(const HttpResponse &other);
	HttpResponse& operator=(const HttpResponse &other);

};

std::ostream	&operator<<(std::ostream &os, const HttpResponse &resp);

#endif /* __HTTP_RESPONSE_HPP__ */
