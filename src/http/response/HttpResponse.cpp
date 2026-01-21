/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:00 by achu              #+#    #+#             */
/*   Updated: 2026/01/20 17:07:16 by achu             ###   ########.fr       */
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
	reset();
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

ConnEvent		HttpResponse::build(const HttpRequest &pReq, IWritableNotifier &notifier)
{
	_request = pReq;
	// TODO: Check request error

	std::string	path = _request.getPath();

	_location =	_server.findLocation(subLocaPath(path));
	if (!_location) {
		setError(http::SC_NOT_FOUND);
		return ConnEvent::none();
	}

	if (_location->redirect) {
		addHeader("Location", _location->redirect.get()->path);
		_response.setStatusCode(_location->redirect.get()->code); //TODO: check for none

		//Add 301 or 302 redirect // 302 by default
		if (_location->redirect && _location->redirect.get()->code == http::SC_MOVED_PERMANENTLY) {
			_response.setStatusCode(_location->redirect.get()->code);
		} else {
			//A verifier? MOVE_TEMPORARILY
			_response.setStatusCode(http::SC_FOUND);
		return ;
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
	case http::MTH_HEAD:	handleHEAD(); break;
	case http::MTH_POST:	handlePOST(); break;
	case http::MTH_PUT:		handlePUT(); break;
	case http::MTH_DELETE:	handleDELETE(); break;
	default:
		setError(http::SC_NOT_IMPLEMENTED);
		return ConnEvent::none();
	}

	//TODO: toString
}

bool		HttpResponse::isDone(void) const { return (_isDone);}

// Clean all the variable in the response class to prepare for the next incoming request
void		HttpResponse::reset(void)
{
	_location = NULL;

	_response.statusCode.code = 0;
	_response.statusCode.reason.clear();
	_response.headers.clear();
	_response.body.clear();

	_isDone = false;
}

bool		HttpResponse::isConnectionClose(void) const
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

void	HttpResponse::loadFile(const std::string& pPath)
{
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
		return setError(http::SC_NOT_FOUND);

	if (S_ISDIR(st.st_mode)) {
		//TODO: access folder

		if (path[path.length() - 1] != '/') {
			std::string redirectPath = _request.getPath() + "/";
			addHeader("Location", redirectPath);

			// //Add 301 or 302 redirect // 302 by default
			// if (_location->redirect && _location->redirect.get()->code == http::SC_MOVED_PERMANENTLY) {
			// 	_response.setStatusCode(_location->redirect.get()->code);
			// } else {
			// 	//A verifier? MOVE_TEMPORARILY
			// 	_response.setStatusCode(http::SC_FOUND);
			}
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

void		HttpResponse::handleHEAD(void)
{
	// "The HEAD method is identical to GET except that the server MUST NOT send a message body in the response."
	handleGET();
	_response.body.clear();
}

// TODO: need a Reverse Deque Technique like send but receive instead, might need to redo request 
void		HttpResponse::handlePOST(void)
{
	std::cout << "Handling POST method" << std::endl;

	// Search in _request for Content-Length or Transfer-Encoding: chunked
	// If none found, return 411 Length Required
	std::string contentLength = _request.getHeaderValue("Content-Length");
	std::string transferEncoding = _request.getHeaderValue("Transfer-Encoding");
	std::string contentType = _request.getHeaderValue("Content-Type");

	if (contentLength.empty() && transferEncoding != "chunked")
		return setError(http::SC_LENGTH_REQUIRED);

	if (!contentLength.empty()) {
		if (!isDec(contentLength))
			return setError(http::SC_BAD_REQUEST);

		//A verifer
		// int	clen = std::atoi(contentLength.c_str());
		// if (clen > MAX_BODY_LENGTH)
		// 	return setError(http::SC_CONTENT_TOO_LARGE);
	}

	if (contentType.empty())
		return setError(http::SC_BAD_REQUEST);

	// Handle multipart/form-data //upload
	if (contentType.find("multipart/form-data") != std::string::npos)
	{
		//get upload dir path
		std::string uploadDir;
		if (_location->uploadDir.empty())
			uploadDir = _location->path;
		else
			uploadDir = _location->uploadDir;

		//if uploadDir doesn't exist, error 500
		if (access(uploadDir.c_str(), F_OK) != 0)
		{
			return setError(http::SC_INTERNAL_SERVER_ERROR);
		}
		//write access
		if (access(uploadDir.c_str(), W_OK) != 0)
		{
			return setError(http::SC_FORBIDDEN);
		}


		//SEARCH BOUNDARY
		size_t	boundaryPos = contentType.find("boundary=");
		if (boundaryPos == std::string::npos)
			return setError(http::SC_BAD_REQUEST);
		std::string	boundary = contentType.substr(boundaryPos + 9);
		std::string delimiter = "--" + boundary;

		size_t	pos = 0;
		std::string	body = _request.getBody();

		while ((pos = body.find(delimiter, pos)) != std::string::npos)
		{	
			size_t	nextPos = body.find(delimiter, pos + delimiter.length());
			if (nextPos == std::string::npos)
				break;

			//EXTRACT FILENAME AND CONTENT
			std::string	part = body.substr(pos + delimiter.length(), nextPos - pos - delimiter.length());
			size_t	filenamePos = part.find("filename=\"");
			if (filenamePos != std::string::npos)
			{
				size_t	filenameEnd = part.find("\"", filenamePos + 10);
				if (filenameEnd != std::string::npos)
				{
					//EXTRACT FILENAME
					std::string	filename = part.substr(filenamePos + 10, filenameEnd - filenamePos - 10);

					//CLEAN FILENAME (remove path if any)
					size_t lastSlash = filename.find_last_of("/\\");
					if (lastSlash != std::string::npos)
						filename = filename.substr(lastSlash + 1);
					if (filename.empty())
						continue;

					//EXTRACT CONTENT
					size_t	contentPos = part.find("\r\n\r\n");
					if (contentPos != std::string::npos)
					{
						//EXTRACT CONTENT AND SAVE TO FILE
						std::string	fileContent = part.substr(contentPos + 4, part.length() - contentPos - 6); // -6 to remove trailing \r\n

						//add random time to filename to avoid overwriting
						std::string new_filename;
						size_t ext_pos = filename.find_last_of(".");
						std::ostringstream oss;
						oss << std::time(0);
						if (ext_pos != std::string::npos)
							new_filename = filename.substr(0, ext_pos) + "_" + oss.str() + filename.substr(ext_pos);
						else
							new_filename = filename + "_" + oss.str();

						//SAVE FILE TO filePath
						std::string	filePath = uploadDir + "/" + new_filename;
						std::ofstream	outFile(filePath.c_str(), std::ios::binary);
						if (!outFile)
							return setError(http::SC_INTERNAL_SERVER_ERROR);
						outFile.write(fileContent.c_str(), fileContent.length());
						outFile.close();

						//A verifier? location dans le header si upload
						addHeader("Location", "/" + new_filename);
					}
				}
			}
			pos = nextPos;
		}
		//code 201 Created
		_response.setStatusCode(http::SC_CREATED);
		return;
	}
	// Handle application/x-www-form-urlencoded //forms
	else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		std::istringstream stream(_request.getBody());
		std::string line;
		std::map<std::string, std::string> data;
		while (std::getline(stream, line, '&'))
		{
			// std::cout << line << std::endl;
			size_t pos = line.find('=');
			if (pos != std::string::npos)
			{
				std::string key = url_decode(line.substr(0, pos));
				std::string value = url_decode(line.substr(pos + 1));
				data[key] = value;
			}
		}
		
		for (std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); it++)
		{
			_response.body << "<li><b>" << it->first << "</b>:" << it->second << "</li>\n";
		}
		addHeader("Content-Type", "text/html");
		_response.body = "<html><body><ul>\n" + _response.body + "</ul></body></html>\n";
		_response.setStatusCode(http::SC_OK);
		return;
	}
	// If not multipart/form-data, just save the body to a file
	else
	{
		std::ostringstream oss;
		oss << std::time(0);
		std::string	filePath = _location->path + "/post_data_" + oss.str() + ".txt";
		std::ofstream	outFile(filePath.c_str(), std::ios::binary);
		if (!outFile)
			return setError(http::SC_INTERNAL_SERVER_ERROR);
		outFile.write(_request.getBody().c_str(), _request.getBody().length());
		outFile.close();

		_response.setStatusCode(http::SC_CREATED);
		return;
	}
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
	size_t	written = write(fd, body.c_str(), body.length());
	close(fd);

	if (written < 0)
		return setError(http::SC_INTERNAL_SERVER_ERROR);

	fileExist ? _response.setStatusCode(http::SC_NO_CONTENT) : _response.setStatusCode(http::SC_CREATED);
}

// Delete method to remove an existing file
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
	_response.body.clear();
	_response.statusCode.code = pCode;
	_response.statusCode.reason = HttpData::getStatusType(pCode);

	std::ostringstream	stream;
	stream	<< "<html>\r\n"
			<< "<head><title>" << pCode << " " << HttpData::getStatusType(pCode) << "</title></head>\r\n"
			<< "<body>\r\n"
			<< "<h1>" << pCode << " " << HttpData::getStatusType(pCode) << "</h1>\r\n"
			<< "<p> An error has occured. </p>\r\n"
			<< "</body>\r\n"
			<< "</html>\r\n";
	_response.body = stream.str();

	addHeader("Content-Type", HttpData::getMimeType("text/html"));
	addHeader("Content-Length", longTOstring(_response.body.length()));
}

std::string	HttpResponse::toString(void) const
{
	std::ostringstream	stream;

	stream	<< "HTTP/1.1 " << _response.statusCode.code << " " << _response.statusCode.reason << "\r\n";

	t_headers	headers = _response.headers;
	t_headers::const_iterator	it;
	for (it = headers.begin(); it != headers.end(); it++) {
		stream << it->first << ": " << it->second << "\r\n";
	}
	stream << "\r\n";

	if (!_response.body.empty()) {
		stream << _response.body;
		
	}
}

//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#
