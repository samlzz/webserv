/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkedStream.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 09:36:54 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/20 13:52:25 by achu             ###   ########.fr       */
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

	// forbidden
	ChunkedStream(const ChunkedStream &other);
	ChunkedStream& operator=(const ChunkedStream &other);

public:
	ChunkedStream(): _queue() {}
	virtual ~ChunkedStream() {}

	virtual size_t		size(void) const			{return _queue.size(); }
	virtual bool	hasChunk(void) const			{ return !_queue.empty(); }
	virtual const std::string
					&front(void) const				{ return _queue.front(); }
	virtual void	pop(void)						{ _queue.pop_front(); }

	virtual void	push(const std::string &chunk)	{_queue.push_back(chunk); }
};

#endif /* __CHUNKED_STREAM_HPP__ */