/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConnection.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 15:59:57 by achu              #+#    #+#             */
/*   Updated: 2025/11/29 18:33:41 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpConnection.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

//#****************************************************************************#
//#                             STATIC FUNCTION                                #
//#****************************************************************************#

bool	isRegFile(const std::string& pPath)
{
	struct stat	st;

	if (stat(pPath.c_str(), &st) < 0)
		return (false);
	return (S_ISREG(st.st_mode));
}

bool	isDirectory(const std::string& pPath)
{
	struct stat	st;

	if (stat(pPath.c_str(), &st) < 0)
		return (false);
	return (S_ISDIR(st.st_mode));
}

//#****************************************************************************#
//#                             MEMBER FUNCTION                                #
//#****************************************************************************#

void		HttpConnection::handleGET(void)
{
	if (isDirectory(_request.getPath())) {
		if (_request.getPath().back() != '/')
			return ; // 301 redirect by adding the missing '/'

		std::string		index = _request.getPath() + "index.html";
		if (isRegFile(index))
			return ; // sendFile

		//check for auto index and return if enabled
		return ; // 403
	}

	if (isRegFile(_request.getPath())) {
		return ; //sendFile
	}

	return ; // throw 404
}

/// No body for this one, but the content length must predict
/// how much it would actually send 
void		HttpConnection::handleHEAD(void)
{
	if (isDirectory(_request.getPath())) {
		if (_request.getPath().back() != '/')
			return ; // 301 redirect by adding the missing '/'

		std::string		index = _request.getPath() + "index.html";
		if (isRegFile(index))
			return ; // sendFile

		//check for auto index and return if enabled
		return ; // 403
	}

	if (isRegFile(_request.getPath())) {
		return ; //sendFile
	}

	return ; // throw 404
}

void		HttpConnection::handlePOST(void)
{
	
}

void		HttpConnection::handlePUT(void)
{
	bool			exist = false;
	std::string		path = _request.getPath();

	struct stat	st;
	exist = stat(path.c_str(), &st);

	if (exist && isDirectory(path))
		return ; //403

	if (exist && access(path.c_str(), W_OK) < 0)
		return ; //403

	if (!exist) {
		std::string		parpath = path.substr(0, path.find_last_of('/'));
		if (access(parpath.c_str(), W_OK) < 0)
			return ; // 403
		if (!isDirectory(parpath))
			return ; // 04
	}

	int		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (fd < 0)
		return ; // 500
	size_t	written = write(fd, _request.getBody().c_str(), atoi(_request.getHeaderValue("Content-Length").c_str()));
	close(fd);

	if (written < 0)
		return ; // error

	return ;//(exist ? 201 : 200)
}

void		HttpConnection::handleDELETE(void)
{
	if (isDirectory(_request.getPath()))
		return ; // throw 403 forbidden

	if (isRegFile(_request.getPath()))
		return ; //unlink(_path.c_str()) or other delete file
				 //204 no content no need for a body

	return ; // throw 404
}