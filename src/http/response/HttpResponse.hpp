/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 13:03:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/17 18:09:53 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

# include <stdint.h>

# include "http/HttpTypes.hpp"
# include "http/request/HttpRequest.hpp"
# include "http/response/BuffStream.hpp"
# include "http/response/ResponsePlan.hpp"
# include "http/response/interfaces/IBodySource.hpp"
# include "http/response/interfaces/IMetaSource.hpp"
# include "http/routing/Router.hpp"

# define RESP_MAX_BUF_COUNT		8

# ifndef RESP_MAX_STREAM_SIZE
#  define RESP_BUFFER_SIZE		16 * 1024
# else
#  define RESP_BUFFER_SIZE RESP_MAX_STREAM_SIZE / RESP_MAX_BUF_COUNT
# endif

class HttpResponse {

private:
	struct DecisionContext
	{
		const HttpRequest			&request;
		routing::Context			route;

		DecisionContext(const HttpRequest &req, const routing::Context &routeCtx)
			: request(req), route(routeCtx) {}
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
					const routing::Context &route);
	~HttpResponse();

	// ========================================================================
	// Life Cycle
	// ========================================================================

	bool						fillStream(void);

	// ========================================================================
	// Output Production
	// ========================================================================

	IFifoStreamView<t_bytes>&	stream(void);
	bool						isDone() const;
	bool						shouldCloseConnection(void) const;

	http::e_status_code			getStatus(void) const;

private:

	// ========================================================================
	// Helpers
	// ========================================================================
	void						applyPlan(const ResponsePlan &plan);

	bool						fillMeta(IMetaSource *meta);
	bool						handleCgiRedirect(const std::string &redirectPath);

	void						commitMeta(void);

	std::string					getField(const std::string& pKey) const;

	
	// forbidden
	HttpResponse();
	HttpResponse(const HttpResponse &other);
	HttpResponse& operator=(const HttpResponse &other);

};

#endif /* __HTTP_RESPONSE_HPP__ */
