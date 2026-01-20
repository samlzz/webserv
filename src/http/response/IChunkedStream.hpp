/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IChunkedStream.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 09:29:27 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/20 13:52:53 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ICHUNKED_STREAM_HPP__
# define __ICHUNKED_STREAM_HPP__

# include <string>

// ============================================================================
// Chunked output FIFO stream interface
// ============================================================================
class IChunkedStream {

public:
	virtual ~IChunkedStream() {}

	// ============================================================================
	// Consumer side 
	// ============================================================================

	virtual size_t				size(void) const = 0;

	virtual bool				hasChunk(void) const = 0;

	/**
	 * Access the current front buffer.
	 *
	 * Precondition:
	 * - `hasChunk() == true`
	 *
	 * The returned reference remains valid until `pop()` is called.
	 */
	virtual const std::string&	front(void) const = 0;

	/**
	 * Remove the current front buffer from the stream.
	 *
	 * Precondition:
	 * - `hasChunk() == true`
	 */
	virtual void				pop(void) = 0;

	// ============================================================================
	// Producer side (HttpResponse / CgiConnection)
	// ============================================================================

	/**
	 * Push a new output buffer at the end of the stream.
	 */
	virtual void				push(const std::string& chunk) = 0;
};

#endif /* __ICHUNKED_STREAM_HPP__ */