/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:00 by achu              #+#    #+#             */
/*   Updated: 2026/01/23 02:45:48 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <algorithm>
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
#include "config/validation/configValidate.hpp"

// =========================================================================== //
//                        CONSTRUCTOR & DESTRUCTOR                             //
// =========================================================================== //

HttpResponse::HttpResponse(const Config::Server& pServer)
	: _server(pServer) {
	reset();
}

HttpResponse::~HttpResponse(void) {
	close(fd);
}

// =========================================================================== //
//                            STATIC FUNCTION                                  //
// =========================================================================== //
#pragma region static function

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

// Subsctract the extension part of a URI path or file
static inline std::string	subExt(const std::string& pPath)
{
	std::string		result;
	size_t			start;

	if ((start = pPath.find_last_of('.')) == std::string::npos)
		return ("");

	result = pPath.substr(start + 1, pPath.length() - start);
	if (result.empty())
		return ("");

	return (result);
}

// static inline std::string		subCgiDir(const std::string& pPath)
// {
// 	std::string		result;
// 	size_t			end;

// 	if ((end = pPath.find_first_of('/', 1)) == std::string::npos)
// 		return ("");

// 	result = pPath.substr(0, end);
// 	if (result.empty())
// 		return ("");

// 	return (result);
// }

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

// /// Search every authorized extension in the location config
// /// @return A boolean that authorized or not the uri path extension
// static inline bool		isExtAuth(const std::string &pExt, const Config::t_dict &pExts)
// {
// 	Config::t_dict::const_iterator it;

// 	for (it = pExts.begin(); it != pExts.end(); it++)
// 	{
// 		std::string	ext = it->first;
// 		std::string path = it->second;
// 		if (pExt == ext)
// 			return (true);
// 	}

// 	return (false);
// }

#pragma endregion

// =========================================================================== //
//                            MEMBER FUNCTION                                  //
// =========================================================================== //
#pragma region member function

void	HttpResponse::encode(const char *pbuf, size_t pBufSize) { (void)pbuf; (void)pBufSize; }
void	HttpResponse::finalize(void) {}

ConnEvent		HttpResponse::build(const HttpRequest &pReq, IWritableNotifier &notifier)
{
	(void)notifier;
	_request = pReq;
	if (_request.getStatusCode() != 200) {
		setError(_request.getStatusCode());
		return ConnEvent::none();		
	}

	std::string	path = _request.getPath();

	_location =	_server.findLocation(subLocaPath(path));
	if (!_location) {
		setError(http::SC_NOT_FOUND);
		return ConnEvent::none();
	}

	if (_location->redirect) {
		addHeader("Location", _location->redirect.get()->path);
		_response.setStatusCode(_location->redirect.get()->code); //TODO: check for none
		return ConnEvent::none();
	}

	http::e_method	curMethod = _request.getMethod();
	std::vector<http::e_method>::const_iterator	itMethods;
	itMethods = std::find(_location->methods.begin(), _location->methods.end(), curMethod);
	if (itMethods == _location->methods.end()) {
		addHeader("Allow", methodsTOstring(_location->methods));
		setError(http::SC_METHOD_NOT_ALLOWED);
		return ConnEvent::none();
	}

	bool		isCgi = false;
	std::string	interpreterPath;

	Config::t_dict::const_iterator	it = _location->cgiExts.find(subExt(path));
	if (it != _location->cgiExts.end()) {
		interpreterPath = it->second;
		isCgi = true;
	}

	if (isCgi) {
		if (access((_location->path + path).c_str(), F_OK) != 0) {
			setError(http::SC_NOT_FOUND);
			return ConnEvent::none();
		}

		if (access((_location->path + path).c_str(), R_OK) != 0) {
			setError(http::SC_FORBIDDEN);
			return ConnEvent::none();
		}
		//TODO: CGI stuff
		return ConnEvent::none();
	}
 
	switch (_request.getMethod()) {
	case http::MTH_GET:		handleGET(); break;
	// case http::MTH_HEAD:	handleHEAD(); break;
	// case http::MTH_POST:	handlePOST(); break;
	case http::MTH_PUT:		handlePUT(); break;
	case http::MTH_DELETE:	handleDELETE(); break;
	default:
		setError(http::SC_NOT_IMPLEMENTED);
		return ConnEvent::none();
	}

	_chunkedStream.push(toString(_response));
	return ConnEvent::none();
}

void		HttpResponse::fillStream(void)
{
	char	buffer[__SIZE_OF_CHUNK__];
	while (_chunkedStream.size() <= __MAX_CHUNK__) {
		ssize_t	rd = read(fd, buffer, sizeof(buffer));

		if (rd < 0) {
			_isConnection = false;
			return ;
		}

		if (rd == 0) {
			_isDone = true;
			fd = -1;
			return ;
		}
		_chunkedStream.push(std::string(buffer, rd));
	}
}

// Clean all the variable in the response class to prepare for the next incoming request
void		HttpResponse::reset(void)
{
	_location = NULL;

	_response.statusCode.code = 200;
	_response.statusCode.reason.clear();
	_response.headers.clear();
	_response.body.clear();

	_isDone = false;
	_isConnection = true;
	fd = -1;

	//clear chunked stream too
}

IChunkedStream	&HttpResponse::stream(void) { return (_chunkedStream); }

bool		HttpResponse::isDone(void) const { return (_isDone); }

// Use this function to determine if the socket should be kept for the next request
bool		HttpResponse::shouldCloseConnection(void) const {
	// std::string connection = getHeaderValue("Connection");

	// if (!connection.empty()) {
	// 	if (connection == "close") return (false);
	// 	if (connection == "keep-alive") return (true);
	// }

	// if (_verMaj == "1" && _verMin == "1")
	// 	return (true);

	return (false);
}

#pragma endregion

// =========================================================================== //
//                        PRIVATE MEMBER FUNCTION                              //
// =========================================================================== //
#pragma region private member function

void	HttpResponse::Response::setStatusCode(const int &pCode) {
	statusCode.code = pCode;
	statusCode.reason = HttpData::getStatusType(pCode);
}

void HttpResponse::addHeader(const std::string &pHeader, const std::string &pContent) {
	_response.headers[pHeader] = pContent;
}

void	HttpResponse::loadFile(const std::string& pPath)
{
	struct stat	st;
	stat(pPath.c_str(), &st);
	std::string	ext = subExt(pPath);

	if ((fd = open(pPath.c_str(), O_RDONLY)) < 0) {
		setError(http::SC_FORBIDDEN);
		return;
	}

	addHeader("Content-Length", toString(st.st_size));
	addHeader("Content-Type", HttpData::getMimeType(ext));
}

void		HttpResponse::handleGET(void)
{
	struct stat	st;
	std::string	path = _location->root + _request.getPath();

	std::cout << "hello:" << path << std::endl;

	if (stat(path.c_str(), &st) != 0)
		return setError(http::SC_NOT_FOUND);

	if (S_ISDIR(st.st_mode)) {
		//TODO: access folder

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
				return setError(http::SC_FORBIDDEN);
	
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
	}

	if (S_ISREG(st.st_mode)) {
		loadFile(path);
		return ;
	}

	return setError(http::SC_NOT_FOUND);
}

void		HttpResponse::handlePUT(void)
{
	struct stat		st;
	std::string		path = _request.getPath();

	bool	fileExist = (stat(path.c_str(), &st) == 0);

	if (fileExist && isDirectory(path))
		return setError(http::SC_FORBIDDEN);

	if (fileExist && access(path.c_str(), W_OK) != 0)
		return setError(http::SC_FORBIDDEN);

	if (!fileExist) {
		std::string parent = path.substr(0, path.find_last_of('/'));
		if (stat(parent.c_str(), &st) != 0 || access(parent.c_str(), W_OK) != 0)
			return setError(http::SC_FORBIDDEN);
	}

	int		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return setError(http::SC_INTERNAL_SERVER_ERROR);

	std::string body = _request.getBody();
	ssize_t	written = write(fd, body.c_str(), body.length());
	close(fd);

	if (written < 0)
		return setError(http::SC_INTERNAL_SERVER_ERROR);

	fileExist ? _response.setStatusCode(http::SC_NO_CONTENT) : _response.setStatusCode(http::SC_CREATED);
}

// Delete method to remove an existing filec
void	HttpResponse::handleDELETE(void)
{
	std::string path = _location->path + _request.getPath();
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return setError(http::SC_NOT_FOUND);

	if (S_ISDIR(st.st_mode))
		return setError(http::SC_FORBIDDEN);

	if (unlink(path.c_str()) != 0)
		return setError(http::SC_INTERNAL_SERVER_ERROR);

	_response.setStatusCode(http::SC_NO_CONTENT);
}

void	HttpResponse::setError(int pCode)
{
	std::string		reason = HttpData::getStatusType(pCode);

	_response.body.clear();
	_response.statusCode.code = pCode;
	_response.statusCode.reason = reason;

	std::ostringstream	stream;
	stream	<< "<!DOCTYPE html>"
			<< "<html>\r\n"
			<< "<head><title>" << pCode << " " << reason << "</title></head>\r\n"
			<< "<body>\r\n"
			<< "<h1>" << pCode << " " << reason << "</h1>\r\n"
			<< "<p> An error has occured. </p>\r\n"
			<< "</body>\r\n"
			<< "</html>\r\n";
	_response.body = stream.str();

	addHeader("Content-Type", HttpData::getMimeType("text/html"));
	addHeader("Content-Length", toString(_response.body.length()));
	_chunkedStream.push(toString(_response));
}

std::ostream	&operator<<(std::ostream &pOut, const HttpResponse::Response &pResponse)
{
	pOut << "HTTP/1.1 " << pResponse.statusCode.code << " " << pResponse.statusCode.reason << "\r\n";

	HttpResponse::t_headers	headers = pResponse.headers;
	HttpResponse::t_headers::const_iterator	it;
	for (it = headers.begin(); it != headers.end(); it++) {
		pOut << it->first << ": " << it->second << "\r\n";
	}
	pOut << "\r\n";

	if (!pResponse.body.empty()) {
		pOut << pResponse.body;
	}
	return (pOut);
}


#pragma endregion
