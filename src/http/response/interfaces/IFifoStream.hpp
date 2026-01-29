/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IFifoStream.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 09:29:27 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 11:40:39 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IFIFO_STREAM_HPP__
# define __IFIFO_STREAM_HPP__

# include <cstddef>

// ============================================================================
// Generic consumer FIFO stream interface
// ============================================================================
template <typename T>
class IFifoStreamView {

public:
	virtual ~IFifoStreamView() {}

	virtual size_t		size() const = 0;
	virtual bool		hasBuffer() const = 0;

	/**
	 * Access the current front buffer.
	 *
	 * Precondition:
	 * - `hasChunk() == true`
	 *
	 * The returned reference remains valid until `pop()` is called.
	 */
	virtual const T&	front() const = 0;

	/**
	 * Remove the current front buffer from the stream.
	 *
	 * Precondition:
	 * - `hasChunk() == true`
	 */
	virtual void		pop() = 0;
};

#endif /* __IFIFO_STREAM_HPP__ */