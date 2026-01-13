/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:00 by achu              #+#    #+#             */
/*   Updated: 2026/01/13 22:45:30 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include "HttpResponse.hpp"
#include "http/request/HttpRequest.hpp"
#include "Config.hpp"

// =========================================================================== //
//                        CONSTRUCTOR & DESTRUCTOR                             //
// =========================================================================== //

HttpResponse::HttpResponse(const HttpRequest& pRequest, const Config::Server& pServer)
	: _request(pRequest), _server(pServer) {
	
}

HttpResponse::~HttpResponse(void) {
	
}

// =========================================================================== //
//                            STATIC FUNCTION                                  //
// =========================================================================== //

static std::string		getextension(const std::string& pPath)
{
	std::string		result;
	ssize_t			start;

	if ((start = pPath.find_last_of('.')) == std::string::npos)
		return ("");

	result = pPath.substr(start, pPath.length() - start);
	if (result.empty())
		return ("");

	return (result);
}

static std::string		getCgiDir(const std::string& pPath)
{
	std::string		result;
	ssize_t			end;

	if ((end = pPath.find_first_of('/', 1)) == std::string::npos)
		return ("");

	result = pPath.substr(0, end);
	if (result.empty())
		return ("");

	return (result);
}

static bool		isRegFile(const std::string& pPath)
{
	struct stat	st;

	if (stat(pPath.c_str(), &st) < 0)
		return (false);
	return (S_ISREG(st.st_mode));
}

static bool		isDirectory(const std::string& pPath)
{
	struct stat	st;

	if (stat(pPath.c_str(), &st) < 0)
		return (false);
	return (S_ISDIR(st.st_mode));
}

static inline std::string	trimLocationPath(const std::string &pPath)
{
	if (pPath.empty())
		return ("");

	std::string	result;

	if (pPath[0] != '/')
		result += "/";

	size_t	lastSlash = pPath.find_last_of('/');
	if (lastSlash != std::string::npos)
		result.append(pPath.substr(0, lastSlash));

	return (result);
}

// =========================================================================== //
//                            MEMBER FUNCTION                                  //
// =========================================================================== //

void		HttpResponse::build(void)
{
	std::string	path = _request.getPath();

	_location =	_server.findLocation(trimLocationPath(path));
	if (!_location)
		return (handleERROR(http::SC_NOT_FOUND));

	if (_location->redirect.isSome())
	{
		addHeader("Location", _location->redirect.get()->path);
		_response._statusCode = http::setStatusCode(_location->redirect.get()->code);
	}
}

bool		HttpResponse::isDone(void) const
{
	
}

void		HttpResponse::reset(void)
{
	_location = NULL;
}

bool		HttpResponse::isConnectionClose(void) const
{
	
}

// ---------------------------------------------------------------------
// Output production (transport-driven)
// ---------------------------------------------------------------------

bool		HttpResponse::produceNext(void)
{
	
}

const std::string&		HttpResponse::buffer(void) const
{
	
}

// =========================================================================== //
//                         PRIVATE MEMBER FUNCTION                             //
// =========================================================================== //

void HttpResponse::addHeader(const std::string &pHeader, const std::string &pContent)
{
	_response._headers[pHeader] = pContent;
}


void	HttpResponse::loadFile(const std::string& path, int code)
{
    std::ifstream file(path.c_str(), std::ios::binary);

    if (!file.is_open()) {
        handleERROR(http::SC_FORBIDDEN);
        return;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    _body = ss.str();
    
    _statusCode = code;
    //_headers["Content-Type"] = _getMimeType(path);
}

//CONFIG: this functoin need the config file for cgi_pass
bool		HttpResponse::isCGI(void)
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

void		HttpResponse::handleGET(void)
{
	std::string		path = _request.getPath();

	if (isDirectory(path)) {
		if (path.back() != '/') {
			//_headers["Location"] = path + "/";
			handleERROR(http::SC_MOVED_PERMANENTLY);
			return ;
		}

		std::string index = path + "index.html";
		if (isRegFile(index)) {
			loadFile(index, http::SC_OK);
			return ;
		}

		//CONFIG: check for auto index and return if enabled

		handleERROR(http::SC_FORBIDDEN);
		return ;
	}

	if (isRegFile(path)) {
		loadFile(path, http::SC_OK);
		return ;
	}

	handleERROR(http::SC_NOT_FOUND);
}

/// No body for this one, but the content length must predict
/// how much it would actually send 
void		HttpResponse::handleHEAD(void)
{
	std::string		path = _request.getPath();

	if (isDirectory(path)) {
		if (path.back() != '/') {
			//_headers["Location"] = path + "/";
			handleERROR(http::SC_MOVED_PERMANENTLY);
			return ;
		}

		std::string index = path + "index.html";
		if (isRegFile(index)) {
			loadFile(index, http::SC_OK);
			return ;
		}

		//CONFIG: check for auto index and return if enabled

		handleERROR(http::SC_FORBIDDEN);
		return ;
	}

	if (isRegFile(path)) {
		loadFile(path, http::SC_OK);
		return ;
	}

	handleERROR(http::SC_NOT_FOUND);
}

// Best to create a non overriding PUT method
void		HttpResponse::handlePOST(void)
{
	
	return ; // thorw 405 method not allowed
}

void		HttpResponse::handlePUT(void)
{
	struct stat		st;
	std::string		path = _request.getPath();

	bool	fileExist = (stat(path.c_str(), &st) == 0);

	if (fileExist && isDirectory(path))
		return handleERROR(http::SC_FORBIDDEN);

	if (fileExist && access(path.c_str(), W_OK) != 0)
		return handleERROR(http::SC_FORBIDDEN);

	if (!fileExist) {
		std::string parent = path.substr(0, path.find_last_of('/'));
		if (stat(parent.c_str(), &st) != 0 || access(parent.c_str(), W_OK) != 0)
			return handleERROR(http::SC_FORBIDDEN);
	}

	int		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return handleERROR(http::SC_INTERNAL_SERVER_ERROR);

	std::string body = _request.getBody();
	size_t	written = write(fd, body.c_str(), body.length());
	close(fd);

	if (written < 0)
		return handleERROR(http::SC_INTERNAL_SERVER_ERROR);

	fileExist ? _statusCode = http::SC_NO_CONTENT : _statusCode = http::SC_CREATED;
}

void	HttpResponse::handleDELETE(void)
{
	struct stat st;
	std::string path = _request.getPath();

	if (stat(path.c_str(), &st) != 0)
		return (handleERROR(http::SC_NOT_FOUND));

	if (S_ISDIR(st.st_mode))
		return (handleERROR(http::SC_FORBIDDEN));

	if (access(path.c_str(), W_OK) != 0)
		return (handleERROR(http::SC_FORBIDDEN));

	if (std::remove(path.c_str()) != 0)
		return (handleERROR(http::SC_INTERNAL_SERVER_ERROR));

	_statusCode = http::SC_NO_CONTENT;
}

void		HttpResponse::handleERROR(int pCode)
{
	
}



//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#
