/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:07:04 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/18 14:25:49 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <iostream>
#include <ostream>

#include "config/Config.hpp"
#include "config/validation/configValidate.hpp"
#include "ft_log/ft_log.hpp"
#include "log.h"
#include "server/Exceptions.hpp"
#include "http/cgi/CgiProcess.hpp"
#include "http/dispatch/HttpDispatcher.hpp"
#include "server/Reactor.hpp"
#include "server/connections/ServerConnection.hpp"
#include "ftpp/FtppException.hpp"

#define ERR_USAGE	"Usage: webserv <configuration file>"

extern "C" void signalHandler(int)
{
	Reactor::stop();
}

int main(int ac, char **av)
{
	if (ac != 2)
		return (std::cerr << ERR_USAGE << std::endl, 2);
	srand(std::time(0));

	ft_log::setOutputStream(std::cerr);
	ft_log::enableCategory(WS_LOG);
	ft_log::setLevel(ft_log::LOG_INFO);
	ft_log::setShowLevel(true);
	ft_log::setShowTimestamp(true);

	try {
		Config	conf(av[1]);
#ifdef WS_CONFIG_DEBUG
		conf.print(ft_log::log(WS_LOG_CONFIG, ft_log::LOG_DEBUG));
#endif

		Reactor								pollManager;
		HttpDispatcher						dispatch;
		const std::vector<Config::Server>	&servs = conf.getServers();

		for (size_t i = 0; i < servs.size(); ++i)
			pollManager.addConnection(new ServerConnection(servs[i], dispatch));

		signal(SIGINT, signalHandler);
		pollManager.run();
	}
	catch (const CgiChildExit &cgiExit)
	{
		return (cgiExit.exitCode());
	}
	catch (const FtppException &parseErr)
	{
		ft_log::log(WS_LOG_CONFIG, ft_log::LOG_ERROR)
			<< "Failed to parse " << av[1]
			<< ", check your configuration.\n"
			<< parseErr.what() << std::endl;
		return 2;
	}
	catch (const config_validate::WsConfigError &configErr)
	{
		ft_log::log(WS_LOG_CONFIG, ft_log::LOG_ERROR)
			<< "Invalid configuration detected in " << av[1] << ".\n"
			<< configErr.what() << std::endl;
		return 2;
	}
	catch (const WsServerError &serverErr)
	{
		ft_log::log(WS_LOG_SERVER, ft_log::LOG_ERROR)
			<< "Server critical error, server stop...\n"
			<< serverErr.what() << std::endl;
		return 1;
	}
	catch (const std::exception &e)
	{
		ft_log::log(WS_LOG, ft_log::LOG_ERROR) << "Exception catch: " << e.what();
		return 1;
	}
	return 0;
}