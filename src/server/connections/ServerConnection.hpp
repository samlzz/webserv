/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 13:05:16 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/19 11:25:19 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVERCONNECTION_HPP__
# define __SERVERCONNECTION_HPP__

# include "AConnection.hpp"
# include "config/Config.hpp"
# include "http/dispatch/HttpDispatcher.hpp"
# include "server/ServerCtx.hpp"

# ifndef SERV_LISTEN_QUEUE
#  define LISTEN_QUEUE	100
# endif

class ServerConnection: public AConnection {

private:
	ServerCtx	_ctx;

public:
	ServerConnection(const Config::Server &config,
					const HttpDispatcher &dispatch);

	virtual ConnEvent	handleEvents(short revents);

private:
	// forbidden
	ServerConnection();
	ServerConnection(const ServerConnection& other);
	ServerConnection& operator=(const ServerConnection& other);

};

#endif /* __SERVERCONNECTION_HPP__ */