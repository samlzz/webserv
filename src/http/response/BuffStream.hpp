/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BuffStream.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 11:25:15 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 13:40:40 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __BUFF_STREAM_HPP__
# define __BUFF_STREAM_HPP__

# include <deque>
# include <string>
# include <vector>

# include "interfaces/IFifoStream.hpp"

typedef std::vector<char>	t_bytes;

class BuffStream: public IFifoStreamView<t_bytes> {
private:
	std::deque<t_bytes>	_stream;

public:
	BuffStream(): _stream() {}
	virtual ~BuffStream() {}

	// ============================================================================
	// Consumer side
	// ============================================================================
	virtual size_t			size(void) const				{ return _stream.size(); }
	virtual bool			hasBuffer(void) const			{ return !_stream.empty(); }

	virtual const t_bytes	&front(void) const				{ return _stream.front(); }
	virtual void			pop(void)						{ _stream.pop_front(); }

	// ============================================================================
	// Producer side
	// ============================================================================
	void					push(const t_bytes &buff)	{_stream.push_back(buff); }
	void					push(const char *buff, size_t size)
	{
		push(t_bytes(buff, buff + size));
	}
	void					push(const std::string &data)
	{
		push(t_bytes(data.begin(), data.end()));
	}

private:
	// forbidden
	BuffStream(const BuffStream &other);
	BuffStream& operator=(const BuffStream &other);
};

#endif /* __BUFF_STREAM_HPP__ */
