/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWriteConnection.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:37:42 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 16:48:50 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_WRITE_CONNECTION_HPP__
# define __CGI_WRITE_CONNECTION_HPP__

# include <cstddef>

# include "http/response/BuffStream.hpp"
# include "server/connections/AConnection.hpp"
# include "server/connections/ConnEvent.hpp"

class CgiProcess;

class CgiWriteConnection: public AConnection {

private:
	const t_bytes		&_body;
	size_t				_offset;
	CgiProcess			&_ctx;
	bool				_spawned;

public:
	CgiWriteConnection(int stdinFd, const t_bytes &body, CgiProcess &ctx);
	virtual ~CgiWriteConnection();

	virtual ConnEvent	handleEvents(short revents);
	virtual IConnection	*buddy(void);
	virtual void		detachBuddy(void);

private:
	// forbidden
	CgiWriteConnection();
	CgiWriteConnection(const CgiWriteConnection &other);
	CgiWriteConnection& operator=(const CgiWriteConnection &other);
};

#endif /* __CGI_WRITE_CONNECTION_HPP__ */
