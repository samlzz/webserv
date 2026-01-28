/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWriteConnection.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:37:42 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 16:14:05 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_WRITE_CONNECTION_HPP__
# define __CGI_WRITE_CONNECTION_HPP__

# include <cstddef>
# include <string>

# include "server/connections/AConnection.hpp"
# include "server/connections/ConnEvent.hpp"

class CgiProcess;

class CgiWriteConnection: public AConnection {

private:
	std::string			_body;
	size_t				_offset;
	CgiProcess			&_ctx;
	bool				_spawned;

public:
	CgiWriteConnection(int stdinFd, const std::string &body, CgiProcess &ctx);
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
