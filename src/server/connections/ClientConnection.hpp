/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:47:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 13:03:47 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENTCONNECTION_HPP__
# define __CLIENTCONNECTION_HPP__

# include <cstddef>
# include <ctime>

# include "AConnection.hpp"
# include "http/request/HttpRequest.hpp"
# include "IWritableNotifier.hpp"
# include "http/response/HttpResponse.hpp"
# include "server/ServerCtx.hpp"
# include "server/connections/ConnEvent.hpp"
# include "server/connections/IConnection.hpp"

# ifndef CLIENT_READ_BUF_SIZE
#  define CLIENT_READ_BUF_SIZE	2048
# endif

# define CLIENT_TIMEOUT_ACCEPT	10000	// 10 s
# define CLIENT_TIMEOUT_REQ		60000	// 60s
# define CLIENT_TIMEOUT_RESP	30000	// 30s

class ClientConnection: public AConnection, public IWritableNotifier {

private:

	enum e_client_state
	{
		CS_WAIT_FIRST_BYTE,		// socket accepted
		CS_WAIT_REQUEST,		// read request (or keep-alive)
		CS_WAIT_RESPONSE		// write response
	};

	ServerCtx				&_serv;
	HttpRequest				_req;
	HttpResponse			*_resp;
	size_t					_offset;
	IConnection				*_cgiRead;
	e_client_state			_state;
	time_t					_tsLastActivity;

public:
	ClientConnection(int cliSockFd, ServerCtx &servCtx);

	virtual ConnEvent	handleEvents(short revents);

	virtual ConnEvent	checkTimeout(time_t now);

	virtual void		notifyWritable(void);

	virtual IConnection	*buddy(void);
	virtual void		detachBuddy(void);

private:
	ConnEvent			handleRead(void);
	ConnEvent			handleWrite(void);

	ConnEvent			buildResponse(void);

	time_t				timeoutFromState(void);

	// forbidden
	ClientConnection();
	ClientConnection(const ClientConnection& other);
	ClientConnection& operator=(const ClientConnection& other);
};

#endif /* __CLIENTCONNECTION_HPP__ */