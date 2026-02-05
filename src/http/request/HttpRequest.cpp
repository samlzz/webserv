/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:12 by achu              #+#    #+#             */
/*   Updated: 2026/02/05 16:48:18 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include "HttpRequest.hpp"
#include "http/response/BuffStream.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <string>

#include "http/HttpTypes.hpp"

#define CURRENT_STATE() _state
#define UPDATE_STATE(S) _state = S

//#****************************************************************************#
//#                        CONSTRUCTOR & DESTRUCTOR                            #
//#****************************************************************************#
#pragma region Construct & Destruct

HttpRequest::HttpRequest(const Config::Server& pConfig) 
	: _config(pConfig) {
	reset();
}

HttpRequest::~HttpRequest(void) {
	_buffer.clear();
}

//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#
#pragma region Getter / Setter

http::e_method			HttpRequest::getMethod() const		{ return (_request.method);       };
void					HttpRequest::setMethod(
									http::e_method pMethod)	{ _request.method = pMethod; }

const std::string		&HttpRequest::getPath() const		{ return (_request.uri.path);     };
void					HttpRequest::setPath(const std::string &pPath)
{
	size_t	queryPos = pPath.find('?');
	size_t	fragmentPos = pPath.find('#');

	_request.uri.path = pPath.substr(0, queryPos);
	if (queryPos != std::string::npos)
		_request.uri.query = pPath.substr(queryPos + 1, fragmentPos - queryPos - 1);
	if (fragmentPos != std::string::npos)
		_request.uri.fragment = pPath.substr(fragmentPos + 1);
}

const std::string		&HttpRequest::getQuery() const		{ return (_request.uri.query);    };
const std::string		&HttpRequest::getFragment() const	{ return (_request.uri.fragment); };
int						HttpRequest::getVerMaj() const		{ return (_request.verMaj);       };
int						HttpRequest::getVerMin() const		{ return (_request.verMin);       };
const http::t_headers	&HttpRequest::getHeaders() const 	{ return (_request.headers);      };
const t_bytes			&HttpRequest::getBody() const		{ return (_request.body);         };
http::e_status_code		HttpRequest::getStatusCode() const	{ return (_code);                 };

void	HttpRequest::setField(const std::string& pKey, const std::string& pValue) {
	_request.headers[pKey] = pValue;
}

bool	HttpRequest::hasField(const std::string &pKey) const
{
	http::t_headers	headers = _request.headers;
	http::t_headers::const_iterator	it = headers.find(pKey);

	if (it == headers.end())
		return (false);

	return (true);
};

std::string		HttpRequest::getField(const std::string& pKey) const
{
	http::t_headers	headers = _request.headers;
	http::t_headers::const_iterator	it = headers.find(pKey);

	if (it == headers.end())
		return ("");

	return (it->second);
};

#pragma endregion

//#****************************************************************************#
//#                             STATIC FUNCTION                                #
//#****************************************************************************#
#pragma region Static Function

static bool		isDec(const std::string& pStr)
{
	if (pStr.empty())
		return (false);

	return (pStr.find_first_not_of("0123456789") == std::string::npos);
}

static bool		isHex(const std::string& pStr)
{
	if (pStr.empty())
		return (false);

	return (pStr.find_first_not_of("0123456789ABCDEFabcdef") == std::string::npos);
}

// (string) Hexadecimal to (int) Decimal 
static int		htod(const std::string& pHex)
{
	if (pHex.empty())
		return -1;

	int result = 0;

	for (size_t i = 0; i < pHex.size(); ++i)
	{
		char c = pHex[i];
		result = result * 16;

		if (c >= '0' && c <= '9') result += c - '0';
		else if (c >= 'A' && c <= 'F') result += c - 'A' + 10;
		else if (c >= 'a' && c <= 'f') result += c - 'a' + 10;
		else return (-1);
	}
	return (result);
}

// ========================================================================== //
//                             MEMBER FUNCTION                                //
// ========================================================================== //
#pragma region Member Function

/// @brief None-blocking function using a state machine that parse all the incoming request from client to proceed
/// @param pBuffer Insert the socket incoming buffer string
/// @param pSize Insert the socket incoming buffer size
/// @return Return the number of byte size_t treated in the function
void	HttpRequest::feed(char *pBuffer, size_t pSize)
{
	char	ch;

	for (size_t i = 0; i < pSize; ++i)
	{
		ch = pBuffer[i];
		switch (CURRENT_STATE()) {
		case LINE_METHOD: {
			_tsStart = std::time(0);
			if (ch != ' ') {
				if (_buffer.length() > MAX_METHOD_LENGTH)
					return setError(http::SC_BAD_REQUEST);
				_buffer += ch;
				break;
			}

			if (_buffer == "GET")			_request.method = http::MTH_GET;
			else if (_buffer == "HEAD")		_request.method = http::MTH_HEAD;
			else if (_buffer == "POST")		_request.method = http::MTH_POST;
			else if (_buffer == "PUT")		_request.method = http::MTH_PUT;
			else if (_buffer == "DELETE")	_request.method = http::MTH_DELETE;
			else return setError(http::SC_NOT_IMPLEMENTED);

			UPDATE_STATE(LINE_SPACE_BEFORE_URI);
			_buffer.clear();

			__attribute__ ((fallthrough));
		}

		case LINE_SPACE_BEFORE_URI:
			if (ch != ' ')
				return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(LINE_URI_SLASH);
			break;

		case LINE_URI_SLASH:
			if (ch != '/') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(LINE_URI_PATH);
			break;

		case LINE_URI_PATH: {
			if (ch == ' ') {
				_request.uri.path = _buffer;
				UPDATE_STATE(LINE_SPACE_BEFORE_VER);
				_buffer.clear();
				__attribute__ ((fallthrough));
			}
			else if (ch == '?' || ch == '#') {
				_request.uri.path = _buffer;
				UPDATE_STATE(ch == '?' ? LINE_URI_QUERY : LINE_URI_FRAGMENT);
				_buffer.clear();
				break;
			}
			else {
				if (_buffer.length() > MAX_URI_LENGTH)
					return setError(http::SC_URI_TOO_LONG);
				_buffer += ch;
				break;
			}
		}

		case LINE_URI_QUERY: {
			if (ch == ' ') {
				_request.uri.query = _buffer;
				UPDATE_STATE(LINE_SPACE_BEFORE_VER);
				_buffer.clear();
				__attribute__ ((fallthrough));
			}
			else if (ch == '#') {
				_request.uri.query = _buffer;
				UPDATE_STATE(LINE_URI_FRAGMENT);
				_buffer.clear();
				break;
			}
			else {
				if (_buffer.length() > MAX_URI_LENGTH)
					return setError(http::SC_URI_TOO_LONG);
				_buffer += ch;
				break;
			}
		}

		case LINE_URI_FRAGMENT: {
			if (ch == ' ') {
				_request.uri.fragment = _buffer;
				UPDATE_STATE(LINE_SPACE_BEFORE_VER);
				_buffer.clear();
				__attribute__ ((fallthrough));
			}
			else if (ch == '?')
				return setError(http::SC_BAD_REQUEST);
			else {
				if (_buffer.length() > MAX_URI_LENGTH)
					return setError(http::SC_URI_TOO_LONG);
				_buffer += ch;
				break;
			}
		}

		case LINE_SPACE_BEFORE_VER:
			if (ch != ' ') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(LINE_HTTP_H);
			break;

		case LINE_HTTP_H:
			if (ch != 'H') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(LINE_HTTP_HT);
			break;

		case LINE_HTTP_HT:
			if (ch != 'T') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(LINE_HTTP_HTT);
			break;

		case LINE_HTTP_HTT:
			if (ch != 'T') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(LINE_HTTP_HTTP);
			break;

		case LINE_HTTP_HTTP:
			if (ch != 'P') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(LINE_HTTP_SLASH);
			break;

		case LINE_HTTP_SLASH:
			if (ch != '/') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(LINE_HTTP_MAJOR_VER);
			break;

		case LINE_HTTP_MAJOR_VER:
			if (!std::isdigit(ch)) return setError(http::SC_BAD_REQUEST);
			_request.verMaj = ch - '0';
			UPDATE_STATE(LINE_HTTP_DOT);
			break;

		case LINE_HTTP_DOT:
			if (ch != '.') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(LINE_HTTP_MINOR_VER);
			break;

		case LINE_HTTP_MINOR_VER:
			if (!std::isdigit(ch)) return setError(http::SC_BAD_REQUEST);
			_request.verMin = ch - '0';
			UPDATE_STATE(LINE_ALMOST_DONE);
			_buffer.clear();
			break;

		case LINE_ALMOST_DONE:
			if (ch != '\r') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(LINE_DONE);
			break;

		case LINE_DONE:
			if (ch != '\n') return setError(http::SC_BAD_REQUEST);
			if (_request.verMaj != 1 || _request.verMin != 1)
				return setError(http::SC_VERSION_NOT_SUPPORTED);
			UPDATE_STATE(HEADER_START);
			break;

		case HEADER_START: {
			if (ch == '\r') {
				UPDATE_STATE(HEADER_END);
				break;
			}
			UPDATE_STATE(HEADER_FIELD_START);
			__attribute__ ((fallthrough));
		}

		case HEADER_FIELD_START: {
			if (!std::isalpha(ch)) return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(HEADER_FIELD);
			__attribute__ ((fallthrough));
		}

		case HEADER_FIELD: {
			if (ch != ':') {
				if (_buffer.length() > MAX_HEADER_LENGTH) return setError(http::SC_HEADER_FIELDS_TOO_LARGE);
				_buffer += ch;
				break;
			}
			UPDATE_STATE(HEADER_COLON);
			__attribute__ ((fallthrough));
		}

		case HEADER_COLON:
			if (ch != ':')
				return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(HEADER_VALUE_START);
			break;

		case HEADER_VALUE_START:
			if (ch == ' ') break;
			UPDATE_STATE(HEADER_VALUE);
			__attribute__ ((fallthrough));

		case HEADER_VALUE: {
			if (ch != '\r') {
				if (_temp.length() > MAX_HEADER_LENGTH)
					return setError(http::SC_HEADER_FIELDS_TOO_LARGE);
				_temp += ch;
				break;
			}
			setField(_buffer, _temp);
			UPDATE_STATE(HEADER_VALUE_ALMOST_DONE);
			_buffer.clear();
			_temp.clear();
			__attribute__ ((fallthrough));
		}

		case HEADER_VALUE_ALMOST_DONE:
			if (ch != '\r')
				return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(HEADER_VALUE_DONE);
			break;

		case HEADER_VALUE_DONE:
			if (ch != '\n')
				return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(HEADER_START);
			break;

		case HEADER_END: {
			if (ch != '\n')
				return setError(http::SC_BAD_REQUEST);
			if (getMethod() == http::MTH_GET || getMethod() == http::MTH_HEAD || getMethod() == http::MTH_DELETE) {
				UPDATE_STATE(PARSING_DONE);
				break;
			}
			UPDATE_STATE(BODY_START);
			__attribute__ ((fallthrough));
		}

		case BODY_START: {
			if (hasField("Transfer-Encoding")) {
				if (getField("Transfer-Encoding") != "chunked")
					return setError(http::SC_NOT_IMPLEMENTED);
				UPDATE_STATE(BODY_TRANSFER_HEXA);
			}

			if (hasField("Content-Length") && !hasField("Transfer-Encoding")) {
				_buffer = getField("Content-Length");
				if (!isDec(_buffer))
					return setError(http::SC_BAD_REQUEST);
				_contentLength = std::atoi(_buffer.c_str());
				if (_contentLength > _config.maxBodySize)
					return setError(http::SC_CONTENT_TOO_LARGE);
				UPDATE_STATE(BODY_CONTENT);
			}

			if (!hasField("Content-Length") && !hasField("Transfer-Encoding"))
				return setError(http::SC_LENGTH_REQUIRED);

			_buffer.clear();
			break;
		}

		case BODY_TRANSFER_HEXA: {
			if (std::isalnum(ch)) {
				_buffer += ch;
				break;
			}

			if (!isHex(_buffer))
				return setError(http::SC_BAD_REQUEST);

			_transferLength = htod(_buffer);
			UPDATE_STATE(BODY_TRANSFER_HEXA_ALMOST_DONE);
			__attribute__ ((fallthrough));
		}

		case BODY_TRANSFER_HEXA_ALMOST_DONE:
			if (ch != '\r') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(BODY_TRANSFER_HEXA_DONE);
			break;

		case BODY_TRANSFER_HEXA_DONE:
			if (ch != '\n') return setError(http::SC_BAD_REQUEST);
			if (_transferLength == 0)
				UPDATE_STATE(BODY_TRANSFER_END);
			else
				UPDATE_STATE(BODY_TRANSFER_DATA);
			break;

		case BODY_TRANSFER_DATA: {
			size_t		available = pSize - i;
			size_t		readbytes = std::min(_transferLength, available);

			if (readbytes > 0) {
				_request.body.insert(_request.body.end(), pBuffer + i, pBuffer + i + readbytes);
				_transferLength -= readbytes;
				i += readbytes - 1;
			}

			if (_transferLength == 0)
				UPDATE_STATE(BODY_TRANSFER_DATA_ALMOST_DONE);

			break;
		}

		case BODY_TRANSFER_DATA_ALMOST_DONE:
			if (ch != '\r') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(BODY_TRANSFER_DATA_DONE);
			break;

		case BODY_TRANSFER_DATA_DONE:
			if (ch != '\n') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(BODY_TRANSFER_END);
			__attribute__ ((fallthrough));
			
		case BODY_TRANSFER_END:
			if (htod(_buffer) != 0) {
				UPDATE_STATE(BODY_TRANSFER_HEXA);
				_buffer.clear();
				break;
			}
			UPDATE_STATE(PARSING_DONE);
			break;

		case BODY_CONTENT: {
			size_t		available = pSize - i;
			size_t		readbytes = std::min(_contentLength, available);

			if (readbytes > 0) {
				_request.body.insert(_request.body.end(), pBuffer + i, pBuffer + i + readbytes);
				_contentLength -= readbytes;
				i += readbytes - 1;
			}

			if (_contentLength == 0)
				UPDATE_STATE(PARSING_DONE);

			break;
		}

		case PARSING_ERROR:
			break;

		case PARSING_DONE:
			break;
		}
	}
}

bool	HttpRequest::isDone(void) const
{
	return (CURRENT_STATE() == PARSING_DONE || CURRENT_STATE() == PARSING_ERROR);
}

bool	HttpRequest::isError() const
{
	return (CURRENT_STATE() == PARSING_ERROR);
}

// Use this function to reset after the request and response has been send, and the connection is keep-alive
void	HttpRequest::reset(void)
{
	UPDATE_STATE(LINE_METHOD);

	_request.method = http::MTH_UNKNOWN;
	_request.uri.path.clear();
	_request.uri.query.clear();
	_request.uri.fragment.clear();
	_request.verMaj = -1;
	_request.verMin = -1;
	_request.headers.clear();
	_request.body.clear();

	_transferLength = 0;
	_contentLength = 0;

	_tsStart = 0;
	_buffer.clear();
	_code = http::SC_OK;
}

void	HttpRequest::checkTimeout(time_t now)
{
	time_t timeout;

	if (_state < HEADER_END)
		timeout = REQ_TIMEOUT_HEADER;
	else
		timeout = REQ_TIMEOUT_BODY;

	if (_tsStart && difftime(now, _tsStart) > timeout)
		setError(http::SC_REQUEST_TIMEOUT);
}

void HttpRequest::setError(const http::e_status_code pCode)
{
	_code = pCode;
	setField("Connection", "close");
	UPDATE_STATE(PARSING_ERROR);
}

#pragma endregion



//#****************************************************************************#
//#                            OPERATOR OVERLOAD                               #
//#****************************************************************************#

std::ostream &operator<<(std::ostream &pOut, const HttpRequest &pRequest)
{
	pOut << pRequest.getMethod() << " ";
	pOut << pRequest.getPath() << pRequest.getQuery() << pRequest.getFragment() << " ";
	pOut << "HTTP/" << pRequest.getVerMaj() << "." << pRequest.getVerMin();
	pOut << "\r\n";

	http::t_headers headers = pRequest.getHeaders();
	for (http::t_headers::const_iterator it = headers.begin();  it != headers.end(); ++it) {
		pOut << it->first << ": " << it->second << "\r\n";
	}
	pOut << "\r\n";

	const std::vector<char>& body = pRequest.getBody();
	if (!body.empty()) {
		pOut.write(body.data(), body.size());
	}
	pOut << "\r\n";

	pOut << (pRequest.isDone() ? "is done" : "is not done");
	pOut << (pRequest.isError() ? "is error" : "is not error");
	
	return (pOut);
}
