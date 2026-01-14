/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IChunkedStream.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 09:29:27 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/14 10:08:05 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ICHUNKED_STREAM_HPP__
# define __ICHUNKED_STREAM_HPP__

# include <string>

class IChunkedStream {

public:
	virtual ~IChunkedStream() {}

	// ============================================================================
	// Consumer side (ClientConnection)
	// ============================================================================

	virtual bool				hasChunk(void) const = 0;

	// for thoses operations hasChunk must be true, else they has UB
	virtual const std::string&	front(void) const = 0;
	virtual void				pop(void) = 0;

	// ============================================================================
	// Producer side (HttpResponse / CgiConnection)
	// ============================================================================

	virtual void				push(const std::string& chunk) = 0;
};

#endif /* __ICHUNKED_STREAM_HPP__ */