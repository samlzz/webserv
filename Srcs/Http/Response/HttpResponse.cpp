/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:00 by achu              #+#    #+#             */
/*   Updated: 2025/11/24 16:02:51 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request/HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

#define CURRENT_STATE() _state
#define UPDATE_STATE(S) _state = S

//#****************************************************************************#
//#                        CONSTRUCTOR & DESTRUCTOR                            #
//#****************************************************************************#

HttpResponse::HttpResponse(void)
{
	
}

HttpResponse::~HttpResponse(void)
{
	
}

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

void		HttpResponse::sendFile()
{
	
}

void		HttpResponse::sendAutoIndex()
{
	// if autoindex disabled
	// return 403

	// send all the files and dir form vector files
}

void		HttpResponse::handleGET(void)
{
	if (isDirectory(_path)) {
		if (_path.back() != '/')
			return ; // 301 redirect by adding the missing '/'

		std::string		index = _path + "index.html";
		if (isRegFile(index))
			return ; // sendFile

		//check for auto index and return if enabled
		return ; // 403
	}

	if (isRegFile(_path)) {
		return ; //sendFile
	}

	return ; // throw 404
}

/// No body for this one, but the content length must predict
/// how much it would actually send 
void		HttpResponse::handleHEAD(void)
{
	if (isDirectory(_path)) {
		if (_path.back() != '/')
			return ; // 301 redirect by adding the missing '/'

		std::string		index = _path + "index.html";
		if (isRegFile(index))
			return ; // sendFile

		//check for auto index and return if enabled
		return ; // 403
	}

	if (isRegFile(_path)) {
		return ; //sendFile
	}

	return ; // throw 404
}

void		HttpResponse::handlePOST(void)
{
	
}

void		HttpResponse::handlePUT(void)
{
	
}

void		HttpResponse::handleDELETE(void)
{
	if (isDirectory(_path))
		return ; // throw 403 forbidden

	if (isRegFile(_path))
		return ; //unlink(_path.c_str()) or other delete file
				 //204 no content no need for a body

	return ; // throw 404
}

void		HttpResponse::build(const http::e_method& pMethod, const std::string& pPath)
{
	switch (_method)
	{
	case http::MTH_GET:
		handleGET();
		break;
	
	default:
		break;
	}
}

//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#
