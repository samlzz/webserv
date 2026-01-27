/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHttpResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 10:12:02 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 11:46:01 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IHTTP_RESPONSE_HPP__
# define __IHTTP_RESPONSE_HPP__

# include "http/response/BuffStream.hpp"
# include "http/response/IFifoStream.hpp"
# include "server/connections/IWritableNotifier.hpp"

class HttpRequest;
struct ConnEvent;

class IHttpResponse {

public:
	virtual ~IHttpResponse() {}

	// ========================================================================
	// Lifecycle
	// ========================================================================

	/**
	 * Build the HTTP response from a fully parsed request.
	 *
	 * - Must be called exactly once per request
	 * - Initializes headers and internal state
	 * - May push initial buffers into the output stream (e.g. headers)
	 *
	 * @return ConnEvent::NONE  if no extra connection is required
	 * @return ConnEvent::SPAWN if a CGI connection must be spawned
	 */
	virtual ConnEvent			build(const HttpRequest& req, IWritableNotifier &notifier) = 0;

	/**
	 * Reset the response to its initial empty state.
	 * Called after the response has been fully sent.
	 */
	virtual void				reset(void) = 0;

	// ========================================================================
	// Output stream (consumer side)
	// ========================================================================

	/**
	 * Access the output stream of ready-to-send buffers.
	 * ClientConnection consumes this stream.
	 */
	virtual IFifoStreamView<t_bytes>&	stream(void) = 0;

	// ========================================================================
	// Response state
	// ========================================================================

	/**
	 * Indicates whether the response has finished producing all output.
	 * Meaning:
	 * - no more buffers will never be added to the stream (see IChumkEncoder::finalize)
	 */
	virtual bool				isDone(void) const = 0;

	/**
	 * Indicates whether the connection must be closed after this response.
	 * Encapsulates HTTP semantics (version, headers, errors).
	 */
	virtual bool				shouldCloseConnection(void) const = 0;
};

#endif /* __IHTTP_RESPONSE_HPP__ */

