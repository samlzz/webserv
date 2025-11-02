/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:12 by achu              #+#    #+#             */
/*   Updated: 2025/11/02 04:44:53 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.h"
#include <iostream>

#define CURRENT_STATE() _state
#define UPDATE_STATE(S) _state = S

//#****************************************************************************#
//#                        CONSTRUCTOR & DESTRUCTOR                            #
//#****************************************************************************#

HttpRequest::HttpRequest(void)
{
	UPDATE_STATE(REQ_METHOD);
	_contentLength = 0;
}

HttpRequest::~HttpRequest(void)
{
	_buffer.clear();
}

//#****************************************************************************#
//#                             STATIC FUNCTION                                #
//#****************************************************************************#

bool	isDecimal(const std::string& pStr)
{
	if (pStr.empty())
		return (false);

	return (pStr.find_first_not_of("0123456789") == std::string::npos);
}

bool	isHexadecimal(const std::string& pStr)
{
	if (pStr.empty())
		return (false);

	return (pStr.find_first_not_of("0123456789ABCDEF") == std::string::npos);
}

//#****************************************************************************#
//#                             MEMBER FUNCTION                                #
//#****************************************************************************#

void HttpRequest::feed(char *pBuffer, size_t pSize)
{
	char	ch;

	for (size_t i = 0; i < pSize; i++)
	{
		ch = pBuffer[i];
		switch (CURRENT_STATE()) {
		case REQ_METHOD: {
			if (ch != ' ') {
				if (_buffer.length() > MAX_METHOD_LENGTH)
					return ; // throw 501
				_buffer += ch;
				break;
			}

			if (_buffer == "GET")			_method = http::MTH_GET;
			else if (_buffer == "HEAD")		_method = http::MTH_HEAD;
			else if (_buffer == "POST")		_method = http::MTH_POST;
			else if (_buffer == "PUT")		_method = http::MTH_PUT;
			else if (_buffer == "DELETE")	_method = http::MTH_DELETE;
			else return; // throw (_buffer.empty() ? 404 : 501);

			UPDATE_STATE(REQ_SPACE_BEFORE_URI);
			_buffer.clear();
		}

		case REQ_SPACE_BEFORE_URI:
			std::cout << ch;
			if (ch != ' ') return ; //thorw 400
			UPDATE_STATE(REQ_URI_SLASH);
			break;

		case REQ_URI_SLASH:
			if (ch != '/') return ; //thorw 400
			_buffer += ch;
			UPDATE_STATE(REQ_URI_PATH);
			break;

		case REQ_URI_PATH: {
			if (ch == ' ') {
				_path = _buffer;
				UPDATE_STATE(REQ_SPACE_BEFORE_VER);
				_buffer.clear();
				// fallthrough
			}
			else if (ch == '?' || ch == '#') {
				_path = _buffer;
				UPDATE_STATE(ch == '?' ? REQ_URI_QUERY : REQ_URI_FRAGMENT);
				_buffer.clear();
				break;
			}
			else {
				if (_buffer.length() > MAX_URI_LENGTH)
					return; //thorw 414
				_buffer += ch;
				break;
			}
		}

		case REQ_URI_QUERY: {
			if (ch == ' ') {
			 	_query = _buffer;
				UPDATE_STATE(REQ_SPACE_BEFORE_VER);
				_buffer.clear();
				// fallthrough
			}
			else if (ch == '#') {
				_query = _buffer;
				UPDATE_STATE(REQ_URI_FRAGMENT);
				_buffer.clear();
				break;
			}
			else {
				if (_buffer.length() > MAX_URI_LENGTH)
					return; //thorw 414
				_buffer += ch;
				break;
			}
		}

		case REQ_URI_FRAGMENT: {
			if (ch == ' ') {
				_fragment = _buffer;
				UPDATE_STATE(REQ_SPACE_BEFORE_VER);
				_buffer.clear();
				// fallthrough
			}
			else {
				if (_buffer.length() > MAX_URI_LENGTH)
					return; //thorw 414
				_buffer += ch;
				break;
			}
		}

		case REQ_SPACE_BEFORE_VER:
			if (ch != ' ') return ; //thorw 400
			UPDATE_STATE(REQ_HTTP_H);
			break;

		case REQ_HTTP_H:
			if (ch != 'H') return ; //thorw 400
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_HT);
			break;

		case REQ_HTTP_HT:
			if (ch != 'T') return ; //thorw 400
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_HTT);
			break;

		case REQ_HTTP_HTT:
			if (ch != 'T') return ; //thorw 400
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_HTTP);
			break;

		case REQ_HTTP_HTTP:
			if (ch != 'P') return ; //thorw 400
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_SLASH);
			break;

		case REQ_HTTP_SLASH:
			if (ch != '/') return ; //thorw 400
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_MAJOR_VER);
			break;

		case REQ_HTTP_MAJOR_VER:
			if (!std::isdigit(ch)) return ; //thorw 400
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_DOT);
			break;

		case REQ_HTTP_DOT:
			if (ch != '.') return ; //thorw 400
			_buffer += ch;
			UPDATE_STATE(REQ_HTTP_MINOR_VER);
			break;

		case REQ_HTTP_MINOR_VER:
			if (!std::isdigit(ch)) return ; //thorw 400
			_buffer += ch;
			_version = _buffer;
			UPDATE_STATE(REQ_ALMOST_DONE);
			_buffer.clear();
			break;

		case REQ_ALMOST_DONE:
			if (ch != '\r') return ; //thorw 400
			UPDATE_STATE(REQ_DONE);
			break;

		case REQ_DONE:
			if (ch != '\n') return ; //thorw 400
			UPDATE_STATE(HEADER_START);
			break;

		case HEADER_START: {
			if (ch == '\r') {
				UPDATE_STATE(HEADER_END);
				break;
			}
			UPDATE_STATE(HEADER_FIELD_NAME_START);
			break;
		}

		case HEADER_FIELD_NAME_START: {
			if (!std::isalpha(ch)) return; // thorw 400
			UPDATE_STATE(HEADER_FIELD_NAME);
			// fallthrough
		}

		case HEADER_FIELD_NAME: {
			if (ch != ':') {
				if (_buffer.length() > MAX_HEADER_LENGTH) return; // thorw 431
				_buffer += ch;
				break;
			}
			addHeader(_buffer, "");
			UPDATE_STATE(HEADER_FIELD_COLON);
			_buffer.clear();
		}

		case HEADER_FIELD_COLON:
			if (ch != ':') return ; //thorw 400
			UPDATE_STATE(HEADER_FIELD_VALUE_START);
			break;

		case HEADER_FIELD_VALUE_START:
			if (ch != ' ') return ; //thorw 400
			UPDATE_STATE(HEADER_FIELD_VALUE);
			break;

		case HEADER_FIELD_VALUE: {
			if (ch != '\r') {
				if (_buffer.length() > MAX_HEADER_LENGTH) return; // thorw 431
				_buffer += ch;
				break;
			}
			setLastHeader(_buffer);
			UPDATE_STATE(HEADER_FIELD_ALMOST_DONE);
			_buffer.clear();
			// fallthrough
		}

		case HEADER_FIELD_ALMOST_DONE:
			if (ch != '\r') return ; //thorw 400
			UPDATE_STATE(HEADER_FIELD_DONE);
			break;

		case HEADER_FIELD_DONE:
			if (ch != '\n') return ; //thorw 400
			UPDATE_STATE(HEADER_FIELD_NAME_START);
			break;

		case HEADER_END: {
			if (ch != '\n') return; // throw 400
			UPDATE_STATE(BODY_MESSAGE_START);
			break;
		}

		case BODY_MESSAGE_START: {
			if (getMethod() == http::MTH_GET || getMethod() == http::MTH_HEAD) {
				UPDATE_STATE(PARSING_DONE);
				break;
			}

			if (hasHeaderName("Transfer-Encoding")) {
				if (hasHeaderName("Content-Length"))
					return ; // throw 400
				_transferEncoding = getHeaderValue("Transfer-Encoding");
				if (_transferEncoding != "chunked")
					return ; // throw 501
				UPDATE_STATE(BODY_CHUNKED);
				break;
			}

			if (hasHeaderName("Content-Length")) {
				_buffer = getHeaderValue("Content-Length");
				if (!isDecimal(_buffer))
					return ; // 400
				_contentLength = std::atoi(_buffer.c_str());
				UPDATE_STATE(BODY_CONTENT);
				_buffer.clear();
				break;
			}

 			return ; // throw 411
		}

		case BODY_CHUNKED: {
			size_t		remaining = _contentLength;
			size_t		available = pSize - i;
			size_t		readbytes = std::min(remaining, available);

			
			break;
		}

		case BODY_CONTENT: {
			size_t		remaining = _contentLength;
			size_t		available = pSize - i;
			size_t		readbytes = std::min(remaining, available);

			_body.append(pBuffer + i, readbytes);
			_contentLength -= readbytes;
			i += remaining - 1;

			if (_contentLength == 0)
				UPDATE_STATE(PARSING_DONE);
			break;
		}

		case PARSING_DONE:
			break;

		default:
			break;
		}
	}
}

//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#

http::e_method											HttpRequest::getMethod(void) const { return (_method); }
std::string												HttpRequest::getPath(void) const { return (_path); };
std::string												HttpRequest::getQuery(void) const { return (_query); };
std::string												HttpRequest::getFragment(void) const { return (_fragment); };
std::string												HttpRequest::getVersion(void) const { return (_version); }
std::vector<std::pair<std::string, std::string>>		HttpRequest::getHeaders(void) const { return (_headers); }
bool													HttpRequest::hasHeaderName(const std::string &pKey) const {
	for (size_t i = 0; i < _headers.size(); ++i)
	{
		if (_headers[i].first == pKey)
			return (true);
	}
	return (false);
};
std::string												HttpRequest::getHeaderValue(const std::string& pKey) const {
	for (size_t i = 0; i < _headers.size(); ++i)
	{
		if (_headers[i].first == pKey)
			return (_headers[i].second);
	}
	return ("");
};
std::string												HttpRequest::getHeaderValueAt(int pIdx) const {
	if (_headers.size() < pIdx)
		return ("");
	return (_headers[pIdx].second);
}
std::string												HttpRequest::getBody(void) const { return (_body); };
http::e_status_code										HttpRequest::getStatusCode(void) const { return (_status); }


void		HttpRequest::addHeader(const std::string& pKey, const std::string& pValue) {
	_headers.push_back(std::make_pair(pKey, pValue));
}
void		HttpRequest::setLastHeader(const std::string& pValue) { _headers.back().second = pValue; }

//#****************************************************************************#
//#                            OPERATOR OVERLOAD                               #
//#****************************************************************************#

std::ostream &operator<<(std::ostream &pOut, const HttpRequest &pRequest)
{
	pOut << pRequest.getMethod() << " ";
	pOut << pRequest.getPath() << pRequest.getQuery() << pRequest.getFragment() << " ";
	pOut << pRequest.getVersion();

	for (size_t i = 0; i < pRequest.getHeaders().size(); i++)
	{
		std::cout << pRequest.getHeaders()[i].first << ": ";
		std::cout << pRequest.getHeaders()[i].second << std::endl;
	}
	
	return (pOut);
}

//#****************************************************************************#
//#                                EXCEPTION                                   #
//#****************************************************************************#

const char*		HttpRequest::BadRequestException::what() const throw() {
	return ("400 Bad request");
}

const char*		HttpRequest::LengthRequiredException::what() const throw() {
	return ("411 Length Required");
}

const char		*HttpRequest::ContentTooLargeException::what() const throw() {
	return ("413 Content Too Large");
}

const char*		HttpRequest::URITooLongException::what() const throw() {
	return ("414 URI Too Long");
}

const char*		HttpRequest::HeaderFieldsTooLargeException::what() const throw() {
	return ("431 Request Header Fields Too Large");
}

const char*		HttpRequest::NotImplementedException::what() const throw() {
	return ("501 Not Implemented");
}
