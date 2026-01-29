/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:07:04 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 12:54:08 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <exception>
#include <iostream>

#include "config/Config.hpp"
#include "http/dispatch/HttpDispatcher.hpp"
#include "server/Reactor.hpp"
#include "server/connections/ServerConnection.hpp"
#include "ftpp/FtppException.hpp"

#define ERR_USAGE	"Usage: webserv <configuration file>"

int main(int ac, char **av)
{
	if (ac != 2)
		return (std::cerr << ERR_USAGE << std::endl, 2);

	try {
		Config	conf(av[1]);
#ifdef WS_CONFIG_DEBUG
		conf.print(std::cout);
#endif

		Reactor								pollManager;
		HttpDispatcher						dispatch;
		const std::vector<Config::Server>	&servs = conf.getServers();

		for (size_t i = 0; i < servs.size(); ++i)
			pollManager.addConnection(new ServerConnection(servs[i], dispatch));

		pollManager.run();
	}
	catch (const FtppException &parseErr)
	{
		std::cerr
			<< "Failed to parse " << av[1]
			<< ", check your configuration.\n"
			<< parseErr.what() << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception catch: " << e.what();
	}
	return 0;
}