/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 03:48:58 by achu              #+#    #+#             */
/*   Updated: 2026/01/14 16:19:10 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpData.hpp"

// =========================================================================== //
//                        CONSTRUCTOR & DESTRUCTOR                             //
// =========================================================================== //

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
	statusData[415] = "Unsupported Media Type";
	statusData[431] = "Request Header Fields Too Large";

	statusData[500] = "Internal Server Error";
	statusData[501] = "Not Implemented";
	statusData[502] = "Bad Gateway";
	statusData[504] = "Gateway Timeout";
	statusData[505] = "HTTP Version Not Supported";

	// ====== Mime Init ======
	mimeData["html"]	= "text/html";
	mimeData["txt"]		= "text/plain";
	mimeData["css"]		= "text/css";
	mimeData["js"]		= "application/javascript";

	mimeData["png"]		= "image/png";
	mimeData["jpg"]		= "image/jpg";
	mimeData["jpeg"]	= "image/jpeg";
	mimeData["gif"]		= "image/gif";
	mimeData["svg"]		= "image/svg+xml";
	mimeData["ico"]		= "image/x-icon";

	mimeData["mp4"]		= "video/mp4";
	mimeData["mp3"]		= "audio/mpeg";	
	
	// mimeData["json"]	= "application/json";
	// mimeData["xml"]		= "application/xml";

	mimeData["pdf"]		= "application/pdf";
	mimeData["zip"]		= "application/zip";
	mimeData["x-tar"]	= "application/x-tar";
	mimeData["gzip"]	= "application/gzip";

	contentTypeData["application/x-www-form-urlencoded"] = CT_APPLICATION_X_WWW_FORM_URLENCODED;
	contentTypeData["multipart/form-data"] = CT_MULTIPART_FORM_DATA;
	contentTypeData["application/octet-stream"] = CT_BINARY;
	contentTypeData["text/plain"] = CT_TEXT_PLAIN;

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

int HttpData::getContentTypeKind(const std::string &pContentType)
{
	// t_contentTypeData::iterator	it = contentTypeData.find(pContentType);
	// if (it != contentTypeData.end())
	// 	return (it->second);
	// return (CT_UNKNOWN);

	for (t_contentTypeData::iterator it = contentTypeData.begin(); it != contentTypeData.end(); ++it)
	{
		if (pContentType.find(it->first) != std::string::npos)
			return it->second;
	}
	return CT_UNKNOWN;
}