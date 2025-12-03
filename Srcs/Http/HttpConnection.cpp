/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConnection.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 15:59:57 by achu              #+#    #+#             */
/*   Updated: 2025/12/03 18:06:05 by achu             ###   ########.fr       */
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

std::string		getextension(const std::string& pPath)
{
	std::string		result;
	ssize_t			start;

	if (start = pPath.find_last_of('.') == std::string::npos)
		return ("");

	result = pPath.substr(start, pPath.length() - start);
	if (result.empty())
		return ("");

	return (result);
}

std::string		getCgiDir(const std::string& pPath)
{
	std::string		result;
	ssize_t			end;

	if (end = pPath.find_first_of('/', 1) == std::string::npos)
		return ("");

	result = pPath.substr(0, end);
	if (result.empty())
		return ("");

	return (result);
}

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

// this functoin need the config file for cgi_pass
bool		HttpConnection::isCGI(void)
{
	std::string		extension = getextension(_request.getPath());
	std::string		cgiPath = getCgiDir(_request.getPath());

	if (extension.empty() || cgiPath.empty())
		return (false);

	// compare extension with config allowed extension
	// compare cgiPath with config supposed directory for cgi

	return (true);

	// find script cgi
	// is executable access ?
	// what interpreter extension ?
}

void		HttpConnection::handleGET(void)
{
	if (isCGI()) {
		return ; //run CGI
	}

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

// Best to create a non overriding PUT method
void		HttpConnection::handlePOST(void)
{
	if (isCGI()) {
		return ; //run CGI
	}

	return ; // thorw 405 method not allowed
}

void		HttpConnection::handlePUT(void)
{
	struct stat		st;

	std::string		path = _request.getPath();
	bool			exist = stat(path.c_str(), &st);

	if (exist && isDirectory(path))
		return ; //403

	if (exist && access(path.c_str(), W_OK) < 0)
		return ; //403

	if (!exist) {
		std::string		parpath = path.substr(0, path.find_last_of('/'));
		if (access(parpath.c_str(), W_OK) < 0)
			return ; // 403
	}

	int		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (fd < 0)
		return ; // 500
	size_t	written = write(fd, _request.getBody().c_str(), atoi(_request.getHeaderValue("Content-Length").c_str()));
	close(fd);

	if (written < 0)
		return ; // 500

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