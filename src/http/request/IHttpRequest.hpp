/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHttpRequest.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 19:24:52 by achu              #+#    #+#             */
/*   Updated: 2026/01/21 19:39:57 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __I_HTTP_REQUEST_HPP__
#define __I_HTTP_REQUEST_HPP__

#include <string>

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
	virtual void		reset(void) = 0;

	// ========================================================================
	// Request state
	// ========================================================================

	// Indicates whether the request has finished producing all output.
	virtual bool			isDone(void) const = 0;
};

#endif /* __IHTTP_RESPONSE_HPP__ */