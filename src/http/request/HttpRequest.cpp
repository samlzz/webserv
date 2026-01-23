/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:12 by achu              #+#    #+#             */
/*   Updated: 2026/01/23 20:04:04 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

#include "HttpRequest.hpp"
#include "http/HttpStatus.hpp"

#define CURRENT_STATE() _state
#define UPDATE_STATE(S) _state = S

//#****************************************************************************#
//#                        CONSTRUCTOR & DESTRUCTOR                            #
//#****************************************************************************#

HttpRequest::HttpRequest(void)
{
	reset();
}

HttpRequest::~HttpRequest(void) {
	_buffer.clear();
}

//#****************************************************************************#
//#                             STATIC FUNCTION                                #
//#****************************************************************************#

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
void	HttpRequest::feed(char *pBuffer, size_t pSize)
{
	char	ch;

	for (size_t i = 0; i < pSize; i++)
	{
		ch = pBuffer[i];
		switch (CURRENT_STATE()) {
		case REQ_METHOD: {
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

			UPDATE_STATE(REQ_SPACE_BEFORE_URI);
			_buffer.clear();

			__attribute__ ((fallthrough));
		}

		case REQ_SPACE_BEFORE_URI:
			if (ch != ' ')
				return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(REQ_URI_SLASH);
			break;

		case REQ_URI_SLASH:
			if (ch != '/') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(REQ_URI_PATH);
			break;

		case REQ_URI_PATH: {
			if (ch == ' ') {
				_request.uri.path = _buffer;
				UPDATE_STATE(REQ_SPACE_BEFORE_VER);
				_buffer.clear();
				__attribute__ ((fallthrough));
			}
			else if (ch == '?' || ch == '#') {
				_request.uri.path = _buffer;
				UPDATE_STATE(ch == '?' ? REQ_URI_QUERY : REQ_URI_FRAGMENT);
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

		case REQ_URI_QUERY: {
			if (ch == ' ') {
				_request.uri.query = _buffer;
				UPDATE_STATE(REQ_SPACE_BEFORE_VER);
				_buffer.clear();
				__attribute__ ((fallthrough));
			}
			else if (ch == '#') {
				_request.uri.query = _buffer;
				UPDATE_STATE(REQ_URI_FRAGMENT);
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

		case REQ_URI_FRAGMENT: {
			if (ch == ' ') {
				_request.uri.fragment = _buffer;
				UPDATE_STATE(REQ_SPACE_BEFORE_VER);
				_buffer.clear();
				__attribute__ ((fallthrough));
			}
			else {
				if (_buffer.length() > MAX_URI_LENGTH)
					return setError(http::SC_URI_TOO_LONG);
				_buffer += ch;
				break;
			}
		}

		case REQ_SPACE_BEFORE_VER:
			if (ch != ' ') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(REQ_HTTP_H);
			break;

		case REQ_HTTP_H:
			if (ch != 'H') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_HT);
			break;

		case REQ_HTTP_HT:
			if (ch != 'T') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_HTT);
			break;

		case REQ_HTTP_HTT:
			if (ch != 'T') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_HTTP);
			break;

		case REQ_HTTP_HTTP:
			if (ch != 'P') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_SLASH);
			break;

		case REQ_HTTP_SLASH:
			if (ch != '/') return setError(http::SC_BAD_REQUEST);
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_MAJOR_VER);
			break;

		case REQ_HTTP_MAJOR_VER:
			if (!std::isdigit(ch)) return setError(http::SC_BAD_REQUEST);
			_request.verMaj = ch - '0';
			UPDATE_STATE(REQ_HTTP_DOT);
			break;

		case REQ_HTTP_DOT:
			if (ch != '.') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(REQ_HTTP_MINOR_VER);
			break;

		case REQ_HTTP_MINOR_VER:
			if (!std::isdigit(ch)) return setError(http::SC_BAD_REQUEST);
			_request.verMin = ch - '0';
			UPDATE_STATE(REQ_ALMOST_DONE);
			_buffer.clear();
			break;

		case REQ_ALMOST_DONE:
			if (ch != '\r') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(REQ_DONE);
			break;

		case REQ_DONE:
			if (ch != '\n') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(HEADER_START);
			break;

		case HEADER_START: {
			if (ch == '\r') {
				UPDATE_STATE(HEADER_END);
				break;
			}
			UPDATE_STATE(HEADER_FIELD_NAME_START);
			__attribute__ ((fallthrough));
		}

		case HEADER_FIELD_NAME_START: {
			if (!std::isalpha(ch)) return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(HEADER_FIELD_NAME);
			__attribute__ ((fallthrough));
		}

		case HEADER_FIELD_NAME: {
			if (ch != ':') {
				if (_buffer.length() > MAX_HEADER_LENGTH) return setError(http::SC_HEADER_FIELDS_TOO_LARGE);
				_buffer += ch;
				break;
			}
			UPDATE_STATE(HEADER_FIELD_COLON);
			__attribute__ ((fallthrough));
		}

		case HEADER_FIELD_COLON:
			if (ch != ':')
				return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(HEADER_FIELD_VALUE_START);
			break;

		case HEADER_FIELD_VALUE_START:
			if (ch == ' ') break;
			UPDATE_STATE(HEADER_FIELD_VALUE);
			__attribute__ ((fallthrough));

		case HEADER_FIELD_VALUE: {
			if (ch != '\r') {
				if (_valueBuf.length() > MAX_HEADER_LENGTH)
					return setError(http::SC_HEADER_FIELDS_TOO_LARGE);
				_valueBuf += ch;
				break;
			}
			addHeader(_buffer, _valueBuf);
			UPDATE_STATE(HEADER_FIELD_ALMOST_DONE);
			_buffer.clear();
			_valueBuf.clear();
			__attribute__ ((fallthrough));
		}

		case HEADER_FIELD_ALMOST_DONE:
			if (ch != '\r')
				return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(HEADER_FIELD_DONE);
			break;

		case HEADER_FIELD_DONE:
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
			UPDATE_STATE(BODY_MESSAGE_START);
			break;
		}

		case BODY_MESSAGE_START: {
			if (hasHeader("Transfer-Encoding")) {
				// TODO:
				// if (hasHeader("Content-Length"))
				// 	SEND_ERROR(http::SC_BAD_REQUEST);
				_transferEncoding = getHeader("Transfer-Encoding");
				if (_transferEncoding != "chunked")
					return setError(http::SC_NOT_IMPLEMENTED);
				UPDATE_STATE(BODY_CHUNKED_SIZE);
				break;
			}

			if (hasHeader("Content-Length")) {
				_buffer = getHeader("Content-Length");
				if (!isDec(_buffer))
					return setError(http::SC_BAD_REQUEST);
				_contentLength = std::atoi(_buffer.c_str());
				if (_contentLength > MAX_BODY_LENGTH)
					return setError(http::SC_CONTENT_TOO_LARGE);
				UPDATE_STATE(BODY_CONTENT);
				_buffer.clear();
				break;
			}

			return setError(http::SC_LENGTH_REQUIRED);
		}

		case BODY_CHUNKED_SIZE: {
			if (ch != '\r') {
				_buffer += ch;
				break;
			}

			if (!isHex(_buffer))
				return setError(http::SC_BAD_REQUEST);

			_transferLength = htod(_buffer);
			UPDATE_STATE(BODY_CHUNKED_SIZE_ALMOST_DONE);
			_buffer.clear();
			__attribute__ ((fallthrough));
		}

		case BODY_CHUNKED_SIZE_ALMOST_DONE:
			if (ch != '\r') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(BODY_CHUNKED_SIZE_DONE);
			break;

		case BODY_CHUNKED_SIZE_DONE:
			if (ch != '\n') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(BODY_CHUNKED_DATA);
			break;

		case BODY_CHUNKED_DATA: {
			size_t		remaining = _transferLength;
			size_t		available = pSize - i;
			size_t		readbytes = std::min(remaining, available);

			if (_transferLength == 0) {
				UPDATE_STATE(BODY_CHUNKED_ALMOST_DONE);
				break;
			}

			if (readbytes > 0) {
				_request.body.append(pBuffer + i, readbytes);
				_transferLength -= readbytes;
				i += readbytes - 1;
			}

			if (_transferLength == 0)
				UPDATE_STATE(BODY_CHUNKED_DATA_ALMOST_DONE);

			break;
		}

		//TODO: 0 \r\n
		//		\r\n

		case BODY_CHUNKED_DATA_ALMOST_DONE:
			if (ch != '\r') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(BODY_CHUNKED_DATA_DONE);
			break;

		case BODY_CHUNKED_DATA_DONE:
			if (ch != '\n') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(BODY_CHUNKED_SIZE);
			break;
			
		case BODY_CHUNKED_ALMOST_DONE:
			if (ch != '\r') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(BODY_CHUNKED_DONE);
			break;

		case BODY_CHUNKED_DONE:
			if (ch != '\n') return setError(http::SC_BAD_REQUEST);
			UPDATE_STATE(PARSING_DONE);
			break;

		case BODY_CONTENT: {
			size_t		remaining = _contentLength;
			size_t		available = pSize - i;
			size_t		readbytes = std::min(remaining, available);

			_request.body.append(pBuffer + i, readbytes);
			_contentLength -= readbytes;

			i += readbytes;

			if (_contentLength == 0)
				UPDATE_STATE(PARSING_DONE);

			if (readbytes > 0) i--;

			break;
		}

		case PARSING_DONE:
			break;

		case PARSING_ERROR:
			break;
		}
	}
}

// Use this function to check is the request has finished parsing at the end
bool	HttpRequest::isDone(void) const
{
	return (CURRENT_STATE() == PARSING_DONE || CURRENT_STATE() == PARSING_ERROR);
}

// Use this function to reset after the request and response has been send, and the connection is keep-alive
void	HttpRequest::reset(void)
{
	UPDATE_STATE(REQ_METHOD);
	_tsStart = 0;

	_request.method = http::MTH_UNKNOWN;
	_request.uri.path.clear();
	_request.uri.query.clear();
	_request.uri.fragment.clear();
	_request.verMaj = -1;
	_request.verMin = -1;
	_request.headers.clear();
	_request.body.clear();

	_contentLength = 0;
	_transferLength = 0;
	_status = http::SC_OK;
}

void	HttpRequest::checkTimeout(time_t now)
{
	time_t timeout;

	if (_state < HEADER_END)
		timeout = REQ_TIMEOUT_HEADER;
	else
		timeout = REQ_TIMEOUT_BODY;
	if (difftime(now, _tsStart) > timeout)
		setError(http::SC_REQUEST_TIMEOUT);
}

void HttpRequest::setError(const http::e_status_code pCode)
{
	_status = pCode;
	UPDATE_STATE(PARSING_ERROR);
}

#pragma endregion

//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#
#pragma region Getter & Setter

void		HttpRequest::addHeader(const std::string& pKey, const std::string& pValue) {
	_request.headers[pKey] = pValue;
}

bool	HttpRequest::hasHeader(const std::string &pKey) const
{
	t_headers	headers = _request.headers;
	t_headers::const_iterator	it = headers.find(pKey);

	if (it == headers.end())
		return (false);

	return (true);
};

std::string		HttpRequest::getHeader(const std::string& pKey) const
{
	t_headers	headers = _request.headers;
	t_headers::const_iterator	it = headers.find(pKey);

	if (it == headers.end())
		return ("");

	return (it->second);
};

//#****************************************************************************#
//#                            OPERATOR OVERLOAD                               #
//#****************************************************************************#

std::ostream &operator<<(std::ostream &pOut, const HttpRequest &pRequest)
{
	pOut << "State:" <<pRequest.getState() << std::endl;
	pOut << pRequest.getMethod() << " ";
	pOut << pRequest.getPath() << pRequest.getQuery() << pRequest.getFragment() << " ";
	pOut << pRequest.getVerMaj() << "." << pRequest.getVerMin() << std::endl;

	const std::map<std::string, std::string> headers = pRequest.getHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();  it != headers.end(); ++it) {
        pOut << it->first << ": " << it->second << "\n";
    }

	std::cout << pRequest.getBody();
	
	return (pOut);
}