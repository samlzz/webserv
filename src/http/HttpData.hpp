/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 03:48:58 by achu              #+#    #+#             */
/*   Updated: 2026/01/14 04:12:30 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_DATA_HPP__
#define __HTTP_DATA_HPP__

#include <string>
#include <map>

class HttpData {
public:
	HttpData(void);
	~HttpData(void);

	typedef std::map<int, std::string>			StatusData;
	typedef std::map<std::string, std::string>	MimeData;

	static StatusData	statusData;
	static MimeData		mimeData;

	static std::string		getStatusType(const int &pCode);
	static std::string		getMimeType(const std::string &pExt);

};

#endif
