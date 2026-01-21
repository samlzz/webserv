/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 03:48:58 by achu              #+#    #+#             */
/*   Updated: 2026/01/21 17:49:31 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpData.hpp"

// =========================================================================== //
//                        CONSTRUCTOR & DESTRUCTOR                             //
// =========================================================================== //

HttpData::t_statusData HttpData::statusData;
HttpData::t_mimeData   HttpData::mimeData;

void HttpData::initData(void)
{
	// ====== Status Init ======
	statusData[200] = "OK";
	statusData[201] = "Created";
	statusData[204] = "No Content";

	statusData[301] = "Moved Permanently";
	statusData[302] = "Found";

	statusData[400] = "Bad Request";
	statusData[403] = "Forbidden";
	statusData[404] = "Not Found";
	statusData[405] = "Method Not Allowed";
	statusData[411] = "Length Required";
	statusData[413] = "Payload Too Large";
	statusData[414] = "URI Too Long";
	statusData[431] = "Request Header Fields Too Large";

	statusData[500] = "Internal Server Error";
	statusData[501] = "Not Implemented";
	statusData[502] = "Bad Gateway";
	statusData[504] = "Gateway Timeout";
	statusData[505] = "HTTP Version Not Supported";

	// ====== Mime Init ======
	mimeData["html"]	= "text/html";
	mimeData["png"]		= "images/png";
}

// =========================================================================== //
//                             MEMBER FUNCTION                                 //
// =========================================================================== //

std::string	HttpData::getStatusType(const int &pCode)
{
	t_statusData::iterator	it = statusData.find(pCode);
	if (it != statusData.end())
		return (it->second);
	return ("Unknown");
}

std::string	HttpData::getMimeType(const std::string &pExt)
{
	t_mimeData::iterator	it = mimeData.find(pExt);
	if (it != mimeData.end())
		return (it->second);
	return ("application/octet-stream");
}