/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 03:48:58 by achu              #+#    #+#             */
/*   Updated: 2026/01/14 16:19:02 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_DATA_HPP__
#define __HTTP_DATA_HPP__

#include <string>
#include <map>

class HttpData {
public:

	typedef std::map<int, std::string>			t_statusData;
	typedef std::map<std::string, std::string>	t_mimeData;
	typedef std::map<std::string, int> 	t_contentTypeData;
	
	static t_statusData		statusData;
	static t_mimeData		mimeData;
	static t_contentTypeData	contentTypeData;

	static void	initData(void);

	static std::string		getStatusType(const int &pCode);
	static std::string		getMimeType(const std::string &pExt);
	static int				getContentTypeKind(const std::string &pContentType);

	enum HttpBodyKind
	{
		CT_UNKNOWN = 0
		, CT_APPLICATION_X_WWW_FORM_URLENCODED
		, CT_MULTIPART_FORM_DATA
		, CT_BINARY
		, CT_TEXT_PLAIN
	};

	// struct ContentTypePost
	// {
	// 	std::string					mime;
	// 	HttpBodyKind				bodyKind;
	// };
};

#endif
