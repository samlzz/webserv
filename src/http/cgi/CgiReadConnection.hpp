/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiReadConnection.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 15:52:27 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/19 19:08:32 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_READ_CONNECTION_HPP__
# define __CGI_READ_CONNECTION_HPP__

# include "server/connections/AConnection.hpp"
# include "server/connections/ConnEvent.hpp"

# ifndef CGI_READ_BUF_SIZE
#  define CGI_READ_BUF_SIZE 4096
# endif

class CgiProcess;

class CgiReadConnection: public AConnection {

private:
	CgiProcess	&_ctx;
	bool		_writeSpawned;

public:
	CgiReadConnection(int stdoutFd, CgiProcess &ctx);

	virtual ConnEvent	handleEvents(short revents);

private:
	// forbidden
	CgiReadConnection();
	CgiReadConnection(const CgiReadConnection &other);
	CgiReadConnection& operator=(const CgiReadConnection &other);
};

#endif /* __CGI_READ_CONNECTION_HPP__ */
