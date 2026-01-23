/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 03:48:58 by achu              #+#    #+#             */
/*   Updated: 2026/01/23 20:42:33 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpData.hpp"
#include "http/HttpTypes.hpp"

// =========================================================================== //
//                        CONSTRUCTOR & DESTRUCTOR                             //
// =========================================================================== //

namespace http
{

Data::t_statusData 		Data::statusData;
Data::t_mimeData 		Data::mimeData;
Data::t_contentTypeData	Data::contentTypeData;

void Data::initData(void)
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
	statusData[408] = "Request Timeout";
	statusData[409] = "Conflict";
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
	statusData[507] = "Insufficient Storage";

	// ====== Mime Init ======
	mimeData["html"]	= "text/html";
	mimeData["txt"]		= "text/plain";
	mimeData["css"]		= "text/css";
	mimeData["js"]		= "application/javascript";

	mimeData["json"]	= "application/json";
	mimeData["xml"]		= "application/xml";

	mimeData["pdf"]		= "application/pdf";
	mimeData["png"]		= "image/png";
	mimeData["jpg"]		= "image/jpg";
	mimeData["jpeg"]	= "image/jpeg";
	mimeData["gif"]		= "image/gif";
	mimeData["svg"]		= "image/svg+xml";
	mimeData["ico"]		= "image/x-icon";

	mimeData["mp4"]		= "video/mp4";
	mimeData["mp3"]		= "audio/mpeg";	
	
	mimeData["zip"]		= "application/zip";
	mimeData["x-tar"]	= "application/x-tar";
	mimeData["gzip"]	= "application/gzip";

	// ====== Content Type Init ======

	contentTypeData["application/x-www-form-urlencoded"]
		= CT_APPLICATION_X_WWW_FORM_URLENCODED;

	contentTypeData["multipart/form-data"] = CT_MULTIPART_FORM_DATA;

	// --- Text-based payloads ---
	contentTypeData["text/plain"] = CT_TEXT_PLAIN;
	contentTypeData["application/json"] = CT_TEXT_PLAIN;
	contentTypeData["application/xml"] = CT_TEXT_PLAIN;

	// --- Binary / opaque payloads ---
	contentTypeData["application/octet-stream"] = CT_BINARY;

	// Uploads de fichiers bruts (PUT typiquement)
	contentTypeData["application/pdf"] = CT_BINARY;
	contentTypeData["application/zip"] = CT_BINARY;
	contentTypeData["application/x-tar"] = CT_BINARY;
	contentTypeData["application/gzip"] = CT_BINARY;
	contentTypeData["image/png"] = CT_BINARY;
	contentTypeData["image/jpeg"] = CT_BINARY;
	contentTypeData["image/gif"] = CT_BINARY;
	contentTypeData["video/mp4"] = CT_BINARY;
	contentTypeData["audio/mpeg"] = CT_BINARY;

}

// =========================================================================== //
//                             MEMBER FUNCTION                                 //
// =========================================================================== //

std::string	Data::getStatusType(const int &pCode)
{
	t_statusData::iterator	it = statusData.find(pCode);
	if (it != statusData.end())
		return (it->second);
	return ("Unknown");
}

std::string	Data::getMimeType(const std::string &pExt)
{
	t_mimeData::iterator	it = mimeData.find(pExt);
	if (it != mimeData.end())
		return (it->second);
	return ("application/octet-stream");
}

e_body_kind	Data::getContentTypeKind(const std::string &pContentType)
{
	t_contentTypeData::iterator	it = contentTypeData.find(pContentType);
	if (it != contentTypeData.end())
		return (it->second);
	return (CT_UNKNOWN);
}

} // namespace http