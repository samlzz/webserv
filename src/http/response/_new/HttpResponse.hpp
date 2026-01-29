/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 13:03:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 13:50:43 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

# include "http/HttpTypes.hpp"
# include "http/response/BuffStream.hpp"
# include "http/response/ResponsePlan.hpp"
# include "http/response/interfaces/IBodySource.hpp"

# define RESP_MAX_BUF_COUNT		8

# ifndef RESP_MAX_STREAM_SIZE
#  define RESP_BUFFER_SIZE		16 * 1024
# else
#  define RESP_BUFFER_SIZE RESP_MAX_STREAM_SIZE / RESP_MAX_BUF_COUNT
# endif

class HttpResponse {

private:
	http::e_status_code	_status;
	http::t_headers		_headers;
	IBodySource			*_body;
	BuffStream			_out;

	bool				_commited;
	bool				_done;

public:
	// ============================================================================
	// Construction / Destruction
	// ============================================================================
	HttpResponse(const ResponsePlan &plan);
	~HttpResponse();

	// ========================================================================
	// Life Cycle
	// ========================================================================

	virtual void						fillStream(void);

	// ========================================================================
	// Output Production
	// ========================================================================

	virtual IFifoStreamView<t_bytes>&	stream(void);
	virtual bool						isDone() const;
	virtual bool						shouldCloseConnection(void) const;

private:

	// ========================================================================
	// Helpers
	// ========================================================================
	void								commitMeta(void);
	
	// forbidden
	HttpResponse();
	HttpResponse(const HttpResponse &other);
	HttpResponse& operator=(const HttpResponse &other);

};

#endif /* __HTTP_RESPONSE_HPP__ */
