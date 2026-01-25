/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:00 by achu              #+#    #+#             */
/*   Updated: 2026/01/23 21:29:38 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include "config/validation/configValidate.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/response/HttpResponse.hpp"
#include "http/request/HttpRequest.hpp"
#include "config/Config.hpp"
#include "config/validation/configValidate.hpp"
#include "server/connections/ConnEvent.hpp"

// =========================================================================== //
//                        CONSTRUCTOR & DESTRUCTOR                             //
// =========================================================================== //

HttpResponse::HttpResponse(const Config::Server& pServer)
	: _server(pServer)
{
	reset();
}

HttpResponse::~HttpResponse(void)
{
	close(_fd);
}

// =========================================================================== //
//                            STATIC FUNCTION                                  //
// =========================================================================== //
#pragma region static function

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

static inline std::string url_decode(const std::string &str)
{
	std::string result;

	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '%' && i + 2 < str.length())
		{
			int hex_val;
			std::istringstream hex_stream(str.substr(i + 1, 2));
			if (hex_stream >> std::hex >> hex_val)
			{
				result += static_cast<char>(hex_val);
				i += 2;
			}
			else
				result += str[i];
		}
		else if (str[i] == '+')
			result += ' ';
		else
			result += str[i];
	}
	return result;
}
//Remove /../ and /./ in path
std::string normalizePath(const std::string &path)
{
	std::string newPath;
	size_t length = path.length();

	for (size_t i = 0; i < length; )
	{
		// Handle "/../"
		if (i + 3 < length && path[i] == '/' && path[i + 1] == '.' && path[i + 2] == '.' && path[i + 3] == '/')
		{

			size_t pos = newPath.find_last_of('/');
			if (pos != std::string::npos)
				newPath.erase(pos);
			i += 3;
		}
		// Handle "/./"
		else if (i + 2 < length && path[i] == '/' && path[i + 1] == '.' && path[i + 2] == '/')
		{
			i += 2;
		}
		else
		{
			newPath += path[i];
			i++;
		}
	}
	return newPath;
}

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

	std::string normalizedPath = normalizePath(_request.getPath());
	_request.setPath(normalizedPath);
	
	std::string	path = _request.getPath();

	_location = _server.findLocation(path);
	if (!_location) {
		setError(http::SC_NOT_FOUND);
		return ConnEvent::none();
	}

	if (_location->redirect) {
		addHeader("Location", _location->redirect.get()->path);
		//Add 301 or 302 redirect // 302 by default
		if (_location->redirect.get()->code != http::SC_NONE)
			_response.setStatusCode(_location->redirect.get()->code);
		else
			_response.setStatusCode(http::SC_FOUND);
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
		// TODO: CGI stuff
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

	_chunkedStream.push(toString(_response));
	return ConnEvent::none();
}

void		HttpResponse::fillStream(void)
{
	char	buffer[__SIZE_OF_CHUNK__];
	while (_chunkedStream.size() <= __MAX_CHUNK__) {
		ssize_t	rd = read(_fd, buffer, sizeof(buffer));

		if (rd < 0) {
			_isConnection = false;
			return ;
		}

		if (rd == 0) {
			_isDone = true;
			_fd = -1;
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
	_fd = -1;

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
	statusCode.reason = http::Data::getStatusType(pCode);
}

void HttpResponse::addHeader(const std::string &pHeader, const std::string &pContent) {
	_response.headers[pHeader] = pContent;
}

void	HttpResponse::loadFile(const std::string& pPath)
{
	struct stat	st;
	stat(pPath.c_str(), &st);
	std::string	ext = subExt(pPath);

	if ((_fd = open(pPath.c_str(), O_RDONLY)) < 0) {
		setError(http::SC_FORBIDDEN);
		return;
	}

	addHeader("Content-Length", toString(st.st_size));
	addHeader("Content-Type", http::Data::getMimeType(ext));
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

void		HttpResponse::handleHEAD(void)
{
	// "The HEAD method is identical to GET except that the server MUST NOT send a message body in the response."
	handleGET();
	_response.body.clear();
}

bool HttpResponse::validUploadPath(std::string& path)
{
	if (!_location->uploadPath.isSome())
	{
		setError(http::SC_METHOD_NOT_ALLOWED);
		return false;
	}

	path = *_location->uploadPath;

	if (access(path.c_str(), W_OK) != 0)
	{
		setError(http::SC_FORBIDDEN);
		return false;
	}

	if (path[0] == '/')
		path = "." + path;
	return true;
}

std::string createFilename(std::string& filename, http::e_body_kind expression)
{
	std::string new_filename;

	switch (expression)
	{
	case http::CT_BINARY:
		new_filename = "raw_post_" + toString(std::time(0));
		break;
	
	case http::CT_TEXT_PLAIN:
		new_filename = "text_post_" + toString(std::time(0)) + ".txt";
		break;

	case http::CT_APPLICATION_X_WWW_FORM_URLENCODED:
		break;

	case http::CT_MULTIPART_FORM_DATA:
	{
		size_t ext_pos = filename.find_last_of(".");
		if (ext_pos != std::string::npos)
			new_filename = filename.substr(0, ext_pos) + "_" + toString(std::time(0)) + filename.substr(ext_pos);
		else
			new_filename = filename + "_" + toString(std::time(0));
		break;
	}
	case http::CT_UNKNOWN:
		new_filename = "";
		break;

	default:
		new_filename = "";
		break;
	}
	return new_filename;
}

std::string HttpResponse::buildUploadPath(std::string &filename, http::e_body_kind expression)
{
	std::string uploadDir;
	http::e_method	method = _request.getMethod();

	if (method != http::MTH_POST && method != http::MTH_PUT)
		return "";

	if (method == http::MTH_POST)
	{
		if (!validUploadPath(uploadDir))
			return "";
		filename = createFilename(filename, expression);
	}

	else if (method == http::MTH_PUT)
	{
		uploadDir = _location->root;
		filename = _request.getPath();
		if (!filename.empty() && filename[0] == '/')
			filename = filename.substr(1);
	}

	std::string	filePath = uploadDir + "/" + filename;

	if (filePath[0] == '/')
		filePath = "." + filePath;

	return filePath;
}

void HttpResponse::handleMultipart(	http::e_method curMethod)
{
	std::string	contentType = _request.getHeader("Content-Type");

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
		std::cout << "Delimiter found at position: " << pos << std::endl;
		//FIND NEXT DELIMITER
		size_t	nextPos = body.find(delimiter, pos + delimiter.length());
		if (nextPos == std::string::npos)
			break;
		std::cout << "Next Delimiter found at position: " << nextPos << std::endl;
		//EXTRACT FILENAME AND CONTENT
		std::string	part = body.substr(pos + delimiter.length(), nextPos - pos - delimiter.length());
		size_t	filenamePos = part.find("filename=\"");
		if (filenamePos == std::string::npos)
			return setError(http::SC_BAD_REQUEST);
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
					return setError(http::SC_BAD_REQUEST);

				//EXTRACT CONTENT
				size_t	contentPos = part.find("\r\n\r\n");
				if (contentPos != std::string::npos)
				{
					//EXTRACT CONTENT AND SAVE TO FILE
					std::string	fileContent = part.substr(contentPos + 4, part.length() - contentPos - 6);
					std::string filePath = buildUploadPath(filename, http::CT_MULTIPART_FORM_DATA);
					if (filePath.empty())
						return;

					int fd = -1;
					if (curMethod == http::MTH_PUT)
						fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
					else if (curMethod == http::MTH_POST)
						fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644);
					else
						return setError(http::SC_BAD_REQUEST);

					if (fd < 0)
						return setError(http::SC_INTERNAL_SERVER_ERROR);

					ssize_t written = write(fd, fileContent.c_str(), fileContent.length());
					close(fd);

					if (written < 0)
						return setError(http::SC_INTERNAL_SERVER_ERROR);
					
					//A verifier? location dans le header si upload
					addHeader("Location", "/" + filename);
				}
				else
				{
					return setError(http::SC_BAD_REQUEST);
				}
			}
		}
		pos = nextPos;
	}
	//code 201 Created
	_response.setStatusCode(http::SC_CREATED);
	std::cout << "Finished handling multipart/form-data POST request." << std::endl;
	return;
}

void HttpResponse::handleUrlEncoded(http::e_method curMethod)
{
	(void)curMethod;
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
		_response.body += "<li><b>" + it->first + "</b>:" + it->second + "</li>\n";
	}
	addHeader("Content-Type", "text/html");
	_response.body = "<html><body><ul>\n" + _response.body + "</ul></body></html>\n";
	_response.setStatusCode(http::SC_OK);

	return;
}

void HttpResponse::handleOctetStream(http::e_method curMethod)
{
	std::string	filename;
	std::string filePath = buildUploadPath(filename, http::CT_BINARY);
	if (filePath.empty())
		return;

	int fd = -1;
	if (curMethod == http::MTH_PUT)
		fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (curMethod == http::MTH_POST)
		fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644);
	else
	 	return setError(http::SC_BAD_REQUEST);
	if (fd < 0)
		return setError(http::SC_INTERNAL_SERVER_ERROR);

	ssize_t written = write(fd, _request.getBody().c_str(), _request.getBody().length());
	close(fd);

	if (written < 0)
		return setError(http::SC_INTERNAL_SERVER_ERROR);

	//A verifier? location dans le header si upload
	addHeader("Location", "/" + filename);

	//code 201 Created
	_response.setStatusCode(http::SC_CREATED);
	return;
}

void HttpResponse::handleTextPlain(http::e_method curMethod)
{
	// Just save the body to a file
	// std::string	uploadDir;
	std::string	filename;
	std::string filePath = buildUploadPath(filename, http::CT_TEXT_PLAIN);
	if (filePath.empty())
		return;

	// std::ofstream	outFile(filePath.c_str(), std::ios::binary);
	int fd = -1;
	if (curMethod == http::MTH_PUT)
		fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (curMethod == http::MTH_POST)
		fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644);
	else
		return setError(http::SC_BAD_REQUEST);

	if (fd < 0)
		return setError(http::SC_INTERNAL_SERVER_ERROR);

	ssize_t written = write(fd, _request.getBody().c_str(), _request.getBody().length());
	close(fd);

	if (written < 0)
		return setError(http::SC_INTERNAL_SERVER_ERROR);

	//A verifier? location dans le header si upload
	addHeader("Location", "/" + filename);

	//code 201 Created
	_response.setStatusCode(http::SC_CREATED);
	return;
}

// TODO: need a Reverse Deque Technique like send but receive instead, might need to redo request 
void		HttpResponse::handlePOST(void)
{
	std::cout << "Handling POST request..." << std::endl;
	std::string contentType = _request.getHeader("Content-Type");
	if (contentType.empty())
		contentType = "application/octet-stream";

	http::e_method curMethod = _request.getMethod();

	switch (http::Data::getContentTypeKind(contentType))
	{
		case http::CT_APPLICATION_X_WWW_FORM_URLENCODED:
			handleUrlEncoded(curMethod);
			break;

		case http::CT_MULTIPART_FORM_DATA:
			handleMultipart(curMethod);
			break;

		case http::CT_BINARY:
			handleOctetStream(curMethod);
			break;

		case http::CT_TEXT_PLAIN:
			handleTextPlain(curMethod);
			break;

		default:
			return setError(http::SC_UNSUPPORTED_MEDIA_TYPE);
	}
}

// TODO: check content type ? see handlePost
void		HttpResponse::handlePUT(void)
{
	struct stat		st;
	// std::string		path = _request.getPath();
	std::string		path = _location->root + _request.getPath();

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

	// int		fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	// if (fd < 0)
	// 	return setError(http::SC_INTERNAL_SERVER_ERROR);

	// std::string body = _request.getBody();
	// ssize_t	written = write(fd, body.c_str(), body.length());
	// close(fd);

	// if (written < 0)
	// 	return setError(http::SC_INTERNAL_SERVER_ERROR);

	handlePOST();

	fileExist ? _response.setStatusCode(http::SC_NO_CONTENT) : _response.setStatusCode(http::SC_CREATED);
}

// Delete method to remove an existing filec
void	HttpResponse::handleDELETE(void)
{
	// std::string path = _location->path + _request.getPath();
	std::string path = _location->root + _request.getPath();
	struct stat st;

	//Check si le path commence par /
	if (path[0] == '/')
		path = "." + path;

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
	std::string		reason = http::Data::getStatusType(pCode);

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

	addHeader("Content-Type", http::Data::getMimeType("html"));
	addHeader("Content-Length", toString(_response.body.length()));
	_isDone = true;
}

std::ostream	&operator<<(std::ostream &pOut, const HttpResponse::Response &pResponse)
{
	pOut << "HTTP/1.1 "
		<< pResponse.statusCode.code << " " << pResponse.statusCode.reason
		<< "\r\n";

	HttpResponse::t_headers					headers = pResponse.headers;
	HttpResponse::t_headers::const_iterator	it;
	for (it = headers.begin(); it != headers.end(); it++) {
		pOut << it->first << ": " << it->second << "\r\n";
	}
	pOut << "\r\n";


	if (!pResponse.body.empty())
		pOut << pResponse.body;

	return (pOut);
}

#pragma endregion
