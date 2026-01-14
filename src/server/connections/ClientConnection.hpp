/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:47:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/14 09:45:55 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENTCONNECTION_HPP__
# define __CLIENTCONNECTION_HPP__

# include <cstddef>

# include "config/Config.hpp"
# include "AConnection.hpp"
# include "http/request/HttpRequest.hpp"
# include "IWritableNotifier.hpp"

# define RECV_BUF_SIZE	2048

class ClientConnection: public AConnection, public IWritableNotifier {

private:
	const Config::Server	&_conf;
	HttpRequest				_req;
//	HttpResponse			_resp;
	size_t					_offset;

	// forbidden
	ClientConnection();
	ClientConnection(const ClientConnection& other);
	ClientConnection& operator=(const ClientConnection& other);

	bool	handleRead(void);
	bool	handleWrite(void);

public:
	ClientConnection(int cliSockFd, const Config::Server &config);

	virtual ConnEvent	handleEvents(short revents);
	virtual void		notifyWritable(void);
};

#endif /* __CLIENTCONNECTION_HPP__ */