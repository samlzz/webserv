/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   non_blocking_serv.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 15:07:50 by achu              #+#    #+#             */
/*   Updated: 2025/12/07 12:31:17 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>

#define LISTEN_QUEUE	5
#define POLL_TIMEOUT	36000
#define BUF_SIZE		4096

typedef std::vector<struct pollfd>	t_pollfdList;

int set_nonblocking(int fd)
{
	int32_t flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void	close_sock_fds(const t_pollfdList &list)
{
	for (t_pollfdList::const_iterator it = list.begin(); it != list.end(); ++it)
		close(it->fd);
}

void	rm_connexion(t_pollfdList &list, int32_t &idx, bool wprint = true)
{
	int32_t	fd = list[idx].fd;

	if (wprint)
		std::cout << "--- Client disconnected (fd: " << fd << ") ---" << std::endl;
	close(fd);
	list.erase(list.begin() + idx);
	--idx;
}

int	main()
{
	int32_t	servSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (servSocket < 0)
		return (perror("webserv: socket"), 1);

	int opt = 1;
	setsockopt(servSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (set_nonblocking(servSocket) < 0)
		return (perror("webserv: fcntl"), close(servSocket), 1);

	sockaddr_in	serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(servSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		return (perror("webser: bind"), close(servSocket), 1);
	if (listen(servSocket, LISTEN_QUEUE) < 0)
		return (perror("webser: listen"), close(servSocket), 1);

	std::cout << "Server listening on port 8080 ..." << std::endl;
	t_pollfdList	pfds;

	pfds.push_back((pollfd){servSocket, POLLIN, 0});
	while (true)
	{
		int32_t	ready = poll(pfds.data(), pfds.size(), POLL_TIMEOUT);
		int32_t	reached = 0;

		if (ready == -1)
			return (perror("webserv: poll"), close_sock_fds(pfds), 1);
		if (ready == 0)
		{
			std::cout << "Poll timeout" << std::endl;
			break;
		}

		for (int32_t i = 0; static_cast<size_t>(i) < pfds.size() && reached < ready; ++i)
		{
			pollfd	sock = pfds[i];
			if (!sock.revents)
				continue;
			reached++;
			if (sock.revents & (POLLNVAL | POLLERR | POLLHUP))
			{
				rm_connexion(pfds, i);
				continue;
			}

			if (sock.fd == servSocket && (sock.revents & POLLIN))
			{
				int32_t	cliFd = accept(servSocket, NULL, NULL);
				if (cliFd < 0)
				{
					if (errno != EWOULDBLOCK && errno != EAGAIN)
						perror("webserv: accept");
					continue;
				}
				if (set_nonblocking(cliFd) < 0)
				{
					perror("webserv: fcntl client");
					close(cliFd);
					continue;
				}
				pfds.push_back((pollfd){cliFd, POLLIN, 0});
				std::cout << "--- New connexion (fd: " << cliFd << ") ---" << std::endl;
			}
			else if (sock.revents & POLLIN)
			{
				char	buf[BUF_SIZE];
				ssize_t	n = recv(sock.fd, buf, sizeof(buf) - 1, 0);

				if (n > 0)
				{
					buf[n] = 0;
					std::cout << "Received from fd " << sock.fd << ":\n" << buf << std::endl;
					pfds[i].events = POLLOUT;
				}
				else if (n == 0)
					rm_connexion(pfds, i);
				else if (n == -1 && errno != EWOULDBLOCK && errno != EAGAIN)
				{
					perror("webserv: recv");
					rm_connexion(pfds, i, false);
				}
			}
			else if (sock.revents & POLLOUT)
			{
				std::string	resp = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
				ssize_t		sent = send(sock.fd, resp.c_str(), resp.length(), 0);

				if (sent < 0 && errno != EWOULDBLOCK && errno != EAGAIN)
					perror("webserv: send");
				else
					std::cout << "Response sent to fd " << sock.fd << std::endl;
				pfds[i].events = POLLIN;
			}
		}
	}
	close_sock_fds(pfds);
	return 0;
}