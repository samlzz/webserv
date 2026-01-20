/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:00 by achu              #+#    #+#             */
/*   Updated: 2026/01/20 02:48:24 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include "http/HttpData.hpp"
#include "http/HttpStatus.hpp"
#include "http/response/HttpResponse.hpp"
#include "http/request/HttpRequest.hpp"
#include "config/Config.hpp"

// =========================================================================== //
//                        CONSTRUCTOR & DESTRUCTOR                             //
// =========================================================================== //

HttpResponse::HttpResponse(const Config::Server& pServer)
	: _server(pServer) {
}

HttpResponse::~HttpResponse(void) {
	
}

// =========================================================================== //
//                            STATIC FUNCTION                                  //
// =========================================================================== //

// Subsctract the location part of a URI path
static inline std::string	subLocaPath(const std::string &pPath)
{
	if (pPath.empty())
		return ("");

	std::string	dirPath;

	if (pPath[0] != '/')
		dirPath += "/";

	size_t	lastSlash = pPath.find_last_of('/');
	if (lastSlash != std::string::npos)
		dirPath.append(pPath.substr(0, lastSlash));

	return (dirPath);
}

// Create a content-value string for the header "Allow"
static inline std::string	methodsTOstring(const std::vector<http::e_method> &pMethods)
{
	std::ostringstream	methods;
	std::vector<http::e_method>::const_iterator	it;

	for (it = pMethods.begin(); it != pMethods.end(); it++)
	{
		methods << *it;

		if (it != pMethods.end() - 1)
			methods << ", ";
	}

	return (methods.str());
}

// Create a content-value string for the header "Content-Length"
static inline std::string	longTOstring(const long pSize) {
	std::ostringstream stream;
	stream << pSize;
	return (stream.str());
}

// Subsctract the extension part of a URI path or file
static inline std::string	subExt(const std::string& pPath)
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

static inline std::string		subCgiDir(const std::string& pPath)
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

/// Search every authorized extension in the location config
/// @return A boolean that authorized or not the uri path extension
static inline bool		isExtAuth(const std::string &pExt, const Config::t_dict &pExts)
{
	Config::t_dict::const_iterator it;

	for (it = pExts.begin(); it != pExts.end(); it++)
	{
		std::string	ext = it->first;
		std::string path = it->second;
		if (pExt == ext)
			return (true);
	}

	return (false);
}

// =========================================================================== //
//                            MEMBER FUNCTION                                  //
// =========================================================================== //

void		HttpResponse::build(const HttpRequest &pReq)
{
	_request = pReq;
	std::string	path = _request.getPath();

	_location =	_server.findLocation(subLocaPath(path));
	if (!_location)
		return (handleERROR(http::SC_NOT_FOUND));

	if (_location->redirect) {
		addHeader("Location", _location->redirect.get()->path);
		_response.setStatusCode(_location->redirect.get()->code); //TODO: check for none
		return ;
	}

	http::e_method	curMethod = _request.getMethod();
	std::vector<http::e_method>::const_iterator	itMethods;
	itMethods = std::find(_location->methods.begin(), _location->methods.end(), curMethod);
	if (itMethods == _location->methods.end()) {
		//TODO: handle error 405 method not allowed
		addHeader("Allow", methodsTOstring(_location->methods));
		return ;
	}

	bool		isCgi = false;
	std::string	interpreterPath;

	for (Config::t_dict::const_iterator itCgiExts = _location->cgiExts.begin(); itCgiExts !=_location->cgiExts.end(); itCgiExts++) {
		if (subExt(path) == itCgiExts->first) {
			interpreterPath = itCgiExts->second;
			isCgi = true;
			break;	
		}
	}

	if (isCgi) {
		if (access((_location->path + path).c_str(), F_OK) != 0) return ; //TODO: handle error 404 not found
		if (access((_location->path + path).c_str(), R_OK) != 0) return ; //TODO: handle error 403 forbidden
		//TODO: CGI stuff
		return ;
	}

	switch (_request.getMethod()) {
	case http::MTH_GET:		handleGET(); break;
	case http::MTH_HEAD:	handleHEAD(); break;
	case http::MTH_POST:	handlePOST(); break;
	case http::MTH_PUT:		handlePUT(); break;
	case http::MTH_DELETE:	handleDELETE(); break;
	default: return; //TODO: handle error 501 not implemented
	}

	//TODO: toString
}

bool		HttpResponse::isDone(void) const
{
	
}

// Clean all the variable in the response class to prepare for the next incoming request
void		HttpResponse::reset(void)
{
	_location = NULL;
	_response.statusCode.code = 0;
	_response.statusCode.reason.clear();
	_response.headers.clear();
	_response.body.clear();
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
//                        PRIVATE MEMBER FUNCTION                              //
// =========================================================================== //

void	HttpResponse::Response::setStatusCode(const int &pCode) {
	statusCode.code = pCode;
	statusCode.reason = HttpData::getStatusType(pCode);
}

void HttpResponse::addHeader(const std::string &pHeader, const std::string &pContent) {
	_response.headers[pHeader] = pContent;
}

// ------------------------------------------

void	HttpResponse::loadFile(const std::string& pPath)
{
	std::ifstream file(pPath.c_str(), std::ios::binary);
	if (!file) {
		handleERROR(http::SC_FORBIDDEN);
		return;
	}

	struct stat	st;
	stat(pPath.c_str(), &st);
	std::string	ext = subExt(pPath);

	addHeader("Content-Length", longTOstring(st.st_size));
	addHeader("Content-Type", HttpData::getMimeType(ext));
}

void		HttpResponse::handleGET(void)
{
	struct stat	st;
	std::string	path = _location->path + _request.getPath();

	if (stat(path.c_str(), &st) != 0)
		return ; //TODO: handle 404

	if (S_ISDIR(st.st_mode)) {
		if (path[path.length() - 1] != '/') {
			std::string redirectPath = _request.getPath() + "/";
			addHeader("Location", redirectPath);
			return ; //TODO: handle 301 MOVED PERMA
		}

		std::string fullIndex = path + _location->index;
		if (isRegFile(fullIndex)) {
			loadFile(fullIndex);
			return ;
		}

		if (_location->autoindex)
		{
			DIR *dir = opendir(path.c_str());
			if (!dir)
				return; // TODO: handle 403 forbidden
	
			struct dirent				*entry;
			std::vector<std::string>	folders;
			std::vector<std::string>	files;

			while ((entry = readdir(dir)) != NULL) {
				struct stat s;
				if (stat(entry->d_name, &s) == 0) {
					if (S_ISDIR(s.st_mode))
						folders.push_back(entry->d_name);
					if (S_ISREG(s.st_mode))
						files.push_back(entry->d_name);
				}
			}
			std::sort(folders.begin(), folders.end());
			std::sort(files.begin(), files.end());

			for (size_t i = 0; i < folders.size(); ++i)
				_response.body += "<a href=\"" + folders[i] + "\">" + folders[i] + "</a><br>\n";
			for (size_t i = 0; i < files.size(); ++i)
				_response.body += "<a href=\"" + files[i] + "\">" + files[i] + "</a><br>\n";

			closedir(dir);
			return;
		}

		handleERROR(http::SC_FORBIDDEN);
		return ;
	}

	if (isRegFile(path)) {
		loadFile(path);
		return ;
	}

	handleERROR(http::SC_NOT_FOUND);
}

void		HttpResponse::handleHEAD(void)
{
	struct stat	st;
	std::string	path = _location->path + _request.getPath();

	if (stat(path.c_str(), &st) != 0)
		return ; //TODO: handle 404

	if (S_ISDIR(st.st_mode)) {
		if (path[path.length() - 1] != '/') {
			std::string redirectPath = _request.getPath() + "/";
			addHeader("Location", redirectPath);
			return ; //TODO: handle 301 MOVED PERMA
		}

		std::string fullIndex = path + _location->index;
		if (isRegFile(fullIndex)) {
			loadFile(fullIndex);
			return ;
		}

		if (_location->autoindex)
		{
			DIR *dir = opendir(path.c_str());
			if (!dir)
				return; // TODO: handle 403 forbidden
	
			struct dirent				*entry;
			std::vector<std::string>	folders;
			std::vector<std::string>	files;

			while ((entry = readdir(dir)) != NULL) {
				struct stat s;
				if (stat(entry->d_name, &s) == 0) {
					if (S_ISDIR(s.st_mode))
						folders.push_back(entry->d_name);
					if (S_ISREG(s.st_mode))
						files.push_back(entry->d_name);
				}
			}
			std::sort(folders.begin(), folders.end());
			std::sort(files.begin(), files.end());

			for (size_t i = 0; i < folders.size(); ++i)
				_response.body += "<a href=\"" + folders[i] + "\">" + folders[i] + "</a><br>\n";
			for (size_t i = 0; i < files.size(); ++i)
				_response.body += "<a href=\"" + files[i] + "\">" + files[i] + "</a><br>\n";

			closedir(dir);
			return;
		}

		handleERROR(http::SC_FORBIDDEN);
		return ;
	}

	if (isRegFile(path)) {
		loadFile(path);
		return ;
	}

	handleERROR(http::SC_NOT_FOUND);
}

// TODO: need a Reverse Deque Technique like send but receive instead, might need to redo request 
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

	fileExist ? _response.setStatusCode(http::SC_NO_CONTENT) : _response.setStatusCode(http::SC_CREATED);
}

// Delete method to remove an existing file
void	HttpResponse::handleDELETE(void)
{
	std::string path = _location->path + _request.getPath();
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return ; //TODO: handle 404

	if (S_ISDIR(st.st_mode))
		return ; //TODO: handle 403

	if (unlink(path.c_str()) != 0)
		return ; //TODO: handle 500

	_response.setStatusCode(http::SC_NO_CONTENT);
}

//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#
