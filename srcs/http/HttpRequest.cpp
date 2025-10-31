/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:12 by achu              #+#    #+#             */
/*   Updated: 2025/10/29 16:15:35 by achu             ###   ########.fr       */
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
	
}

HttpRequest::~HttpRequest(void)
{
	
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
		case REQ_START:
		{
			UPDATE_STATE(REQ_METHOD);
			break;
		}
		case REQ_METHOD:
		{

			switch (expression)
			{
			case constant expression:
				/* code */
				break;
			
			default:
				break;
			}
			UPDATE_STATE(REQ_SPACE_BEFORE_URI);
			break;
		}
		default:
			break;
		}
	}
}

//#****************************************************************************#
//#                              GETTER SETTER                                 #
//#****************************************************************************#

http::e_method			HttpRequest::getMethod(void) const { return (_method); }
std::string				HttpRequest::getPath(void) const { return (_path); };
std::string				HttpRequest::getQuery(void) const { return (_query); };
std::string				HttpRequest::getVersion(void) const { return (_version); };
std::string				HttpRequest::getHeaders(const std::string& pKey) const {
	for (size_t i = 0; i < _headers.size(); ++i)
	{
		if (_headers[i].first == pKey)
			return (_headers[i].second);
	}
	return ("");
};
std::string				HttpRequest::getBody(void) const { return (_body); };

void			HttpRequest::setMethod(const http::e_method& pMethod) { _method = pMethod; }
void			HttpRequest::setPath(const std::string& pPath) { _path = pPath; }
void			HttpRequest::setQuery(const std::string& pQuery) { _query = pQuery; }
void			HttpRequest::setVersion(const std::string& pVersion) { _version = pVersion; }
void			HttpRequest::addHeaders(const std::string& pKey, const std::string& pValue) {
	_headers.push_back(std::make_pair(pKey, pValue));
}
void			HttpRequest::setBody(const std::string& pBody) { _body = pBody; }

//#****************************************************************************#
//#                            OPERATOR OVERLOAD                               #
//#****************************************************************************#

//#****************************************************************************#
//#                                EXCEPTION                                   #
//#****************************************************************************#


