/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 03:48:58 by achu              #+#    #+#             */
/*   Updated: 2026/01/23 20:42:39 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_DATA_HPP__
#define __HTTP_DATA_HPP__

#include "http/HttpTypes.hpp"
#include <string>
#include <map>

namespace http
{
class Data {
public:

	typedef std::map<int, std::string>			t_statusData;
	typedef std::map<std::string, std::string>	t_mimeData;
	typedef std::map<std::string, e_body_kind> 	t_contentTypeData;
	
	static t_statusData			statusData;
	static t_mimeData			mimeData;
	static t_contentTypeData	contentTypeData;

	static void				initData(void);

	static std::string		getStatusType(const int &pCode);
	static std::string		getMimeType(const std::string &pExt);
	static e_body_kind		getContentTypeKind(const std::string &pContentType);
};

} // namespace http

#endif