/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 09:47:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/09 11:16:58 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENTCONNECTION_HPP__
# define __CLIENTCONNECTION_HPP__

# include "config/Config.hpp"
# include "server/AConnection.hpp"

class ClientConnection: public AConnection {

private:
	const Config::Server	&_conf;
	// TODO: append http request and response

	// forbidden
	ClientConnection();
	ClientConnection(const ClientConnection& other);
	ClientConnection& operator=(const ClientConnection& other);

	// TODO: use http request and response
	bool	processIO(short revents);

public:
	ClientConnection(int cliSockFd, const Config::Server &config);

	virtual ConnEvent	handleEvents(short revents);
};

#endif /* __CLIENTCONNECTION_HPP__ */