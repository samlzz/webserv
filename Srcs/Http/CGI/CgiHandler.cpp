/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:07:17 by achu              #+#    #+#             */
/*   Updated: 2025/12/10 17:37:11 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include <unistd.h>

//#****************************************************************************#
//#                        CONSTRUCTOR & DESTRUCTOR                            #
//#****************************************************************************#

CgiHandler::CgiHandler(void) {
	
}

CgiHandler::CgiHandler(HttpRequest pRequest) {
	_method = pRequest.getMethod();
	_path = pRequest.getMethod();
	_query = pRequest.getMethod();
	_header = pRequest.getMethod();
	_body = pRequest.getMethod();
}

CgiHandler::~CgiHandler(void) {
	
}

//#****************************************************************************#
//#                             STATIC FUNCTION                                #
//#****************************************************************************#

char**		buildEnvp(void)
{
	
}

//#****************************************************************************#
//#                             MEMBER FUNCTION                                #
//#****************************************************************************#

void		CgiHandler::runCGI(void)
{
	pid_t	pid;
	int		fd[2];

	if (pipe(fd) < 0)
		return ;

	if (pid = fork() < 0) {
		return ;
	}
	else if (pid == 0) {
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);

		//execve();
		// send message in case of eroor here
	}
	else {
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
	}
}