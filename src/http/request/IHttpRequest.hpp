/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHttpRequest.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 19:24:52 by achu              #+#    #+#             */
/*   Updated: 2026/01/29 16:51:55 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __I_HTTP_REQUEST_HPP__
#define __I_HTTP_REQUEST_HPP__

#include <cstddef>

class IHttpRequest {
public:
	virtual ~IHttpRequest() {}

	// ========================================================================
	// Lifecycle
	// ========================================================================

	// Feed the HTTP Request from an incoming recv().
	// - Can be called multiple time (Non-blocking)
	virtual void		feed(char *pBuffer, size_t pSize) = 0;

	// Reset the request to its initial empty state.
	// Called after the response has been fully sent.
	virtual void		reset() = 0;

	// ========================================================================
	// Request state
	// ========================================================================

	// Indicates whether the request has finished producing all output.
	virtual bool		isDone() const = 0;

	// Indicates whether the request has occured an error inside the parser
	virtual bool		isError() const = 0;
};

#endif