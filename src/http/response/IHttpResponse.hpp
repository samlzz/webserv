/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHttpResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 16:45:40 by achu              #+#    #+#             */
/*   Updated: 2026/01/14 16:31:31 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

class IHttpResponse {
public:
    virtual ~IHttpResponse() {}

    // ---------------------------------------------------------------------
    // Lifecycle
    // ---------------------------------------------------------------------

    /**
     * Build the HTTP response from a fully parsed request.
     * Must be called exactly once per request.
     * After this call, the response is ready to produce output buffers.
     */
    virtual void build(const HttpRequest &pReq) = 0;

    /**
     * Reset the response to its initial empty state.
     */
    virtual void reset(void) = 0;

    // ---------------------------------------------------------------------
    // Output production (transport-driven)
    // ---------------------------------------------------------------------

    /**
     * Prepare the next output buffer to be sent on the socket.
     *
     * This method MUST be called only when the previous buffer
     * has been completely sent.
     *
     * @return true  if a new buffer is available (buffer() becomes non-empty)
     * @return false if no data is available for now (e.g. CGI not ready yet)
     */
    virtual bool produceNext(void) = 0;

    /**
     * Return the current output buffer.
     *
     * The returned buffer MUST remain stable and unchanged until
     * produceNext() is called again.
     *
     * @return a reference to the current buffer (may be empty)
     */
    virtual const std::string& buffer(void) const = 0;

    // ---------------------------------------------------------------------
    // State / flow control
    // ---------------------------------------------------------------------

    virtual bool isDone(void) const = 0;
   
    /**
     * Inside response bc in some case (e.g. timeout or 500 errors)
     * we can want to delete connexion after the response sent
     *
     * It is not only decided by the headers (like in isKeepAlive now)
     */ 
    virtual bool isConnectionClose(void) const = 0;
};