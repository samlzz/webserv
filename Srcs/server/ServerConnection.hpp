/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 13:05:16 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/08 17:14:54 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVERCONNECTION_HPP__
# define __SERVERCONNECTION_HPP__

# include <vector>

# include "AConnection.hpp"
# include "config/Config.hpp"
# include "IConnection.hpp"

# define LISTEN_QUEUE	5

class ServerConnection: public AConnection {

private:
	const Config::Server		&_conf;
	std::vector<IConnection *>	&_pfds;

	// forbidden
	ServerConnection();
	ServerConnection(const ServerConnection& other);
	ServerConnection& operator=(const ServerConnection& other);

public:
	ServerConnection(const Config::Server &config,
						std::vector<IConnection *> &connexions);

	virtual bool handleEvents(short revents);
};

#endif /* __SERVERCONNECTION_HPP__ */