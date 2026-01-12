/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   non_blocking_serv.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 15:07:50 by achu              #+#    #+#             */
/*   Updated: 2025/12/05 22:59:00 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

/*
** In posix standar (or it's came from Linux, i'm not sure),
** when process who own a INET socket exit not longer after owns it,
** the socket it put in a wait mode (for 2 or 4 min) to make sure
** all packets will be send
*/

int	main()
{
	int32_t	servSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (servSocket < 0)
		return (perror("webserv: socket"), 1);

	/* ? Will close socket immediatly even after a shorter exit */
	linger lin;
	lin.l_onoff = 0;
	lin.l_linger = 0;
	setsockopt(servSocket, SOL_SOCKET, SO_LINGER, (const char *)&lin, sizeof(int));

	/* ? Will allow bind to bind the socket even if it is in wait mode */
	// int32_t	reuse = 1;
	// if (setsockopt(servSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
	// 	perror("webserv: setsockopt(SO_REUSEADDR) failed");

	sockaddr_in	serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(servSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		return (perror("webser: bind"), close(servSocket), 1);
	if (listen(servSocket, 5) < 0)
		return (perror("webser: listen"), close(servSocket), 1);

	std::cout << "Server listening on port 8080 ..." << std::endl;

	while (true)
	{
		int32_t	cliFd = accept(servSocket, NULL, NULL);
		if (cliFd < 0)
		{
			perror("webserv: accept");
			continue;
		}

		std::cout << "--- New connexion ---" << std::endl;
		char	buf[4096] = {0};
		size_t	n;

		while ((n = recv(cliFd, buf, sizeof(buf) - 1, 0)) > 0)
		{
			buf[n - 1] = 0;
			std::cout << buf << std::endl;
			send(cliFd, "Hello world !", 13, 0);
		}
		std::cout << "--- Client disconnected ---" << std::endl;
		close(cliFd);
	}

	close(servSocket);
	return 0;
}