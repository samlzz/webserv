/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:47:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/22 11:46:14 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENTCONNECTION_HPP__
# define __CLIENTCONNECTION_HPP__

# include <cstddef>

# include "config/Config.hpp"
# include "AConnection.hpp"
# include "http/request/HttpRequest.hpp"
# include "IWritableNotifier.hpp"
# include "http/response/HttpResponse.hpp"
# include "server/connections/ConnEvent.hpp"
# include "server/connections/IConnection.hpp"

# ifndef CLIENT_READ_BUF_SIZE
#  define CLIENT_READ_BUF_SIZE	2048
# endif

class ClientConnection: public AConnection, public IWritableNotifier {

private:
	HttpRequest				_req;
	HttpResponse			_resp;
	size_t					_offset;
	IConnection				*_cgiRead;

public:
	ClientConnection(int cliSockFd, const Config::Server &config);

	virtual ConnEvent	handleEvents(short revents);

	virtual void		notifyWritable(void);

	virtual IConnection	*buddy(void);
	virtual void		detachBuddy(void);

private:
	ConnEvent			handleRead(void);
	ConnEvent			handleWrite(void);

	// forbidden
	ClientConnection();
	ClientConnection(const ClientConnection& other);
	ClientConnection& operator=(const ClientConnection& other);
};

#endif /* __CLIENTCONNECTION_HPP__ */