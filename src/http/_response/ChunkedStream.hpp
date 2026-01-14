/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkedStream.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 09:36:54 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/14 09:42:57 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHUNKED_STREAM_HPP__
# define __CHUNKED_STREAM_HPP__

# include <deque>
# include <string>

# include "IChunkedStream.hpp"

class ChunkedStream: public IChunkedStream {

private:
	std::deque<std::string>	_queue;

public:
	ChunkedStream(): _queue() {}
	ChunkedStream(const ChunkedStream &other) : _queue(other._queue) {}
	virtual ~ChunkedStream() {}

	ChunkedStream& operator=(const ChunkedStream &other)
	{
		if (this != &other)
			_queue = other._queue;
		return *this;
	}

	virtual bool	hasChunk(void) const			{ return !_queue.empty(); }
	virtual const std::string
					&front(void) const				{ return _queue.front(); }
	virtual void	pop(void)						{ _queue.pop_front(); }

	virtual void	push(const std::string &chunk)	{_queue.push_back(chunk); }
};

#endif /* __CHUNKED_STREAM_HPP__ */
