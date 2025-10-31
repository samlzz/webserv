/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 15:07:50 by achu              #+#    #+#             */
/*   Updated: 2025/10/31 19:43:01 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "HttpRequest.h"

int main()
{
	int	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in		serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(serverSocket, 5);

	int	clientSocket = accept(serverSocket, nullptr, nullptr);

	char	buffer[1024] = {0};
	recv(clientSocket, buffer, sizeof(buffer), 0);

	HttpRequest		clientRequest;
	clientRequest.feed(buffer, sizeof(buffer));
	std::cout << clientRequest << std::endl;

	send(clientSocket, "Va niquer ta mere", 18, 0);

	close(clientSocket);
	close(serverSocket);
	return 0;
}
