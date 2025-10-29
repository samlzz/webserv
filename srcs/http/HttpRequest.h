/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 15:05:14 by achu              #+#    #+#             */
/*   Updated: 2025/10/29 18:43:25 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <string>
#include <unordered_map>

class HttpRequest
{
	private:

		enum	e_method
		{
			MT_GET,
			MT_PUT,
			MT_POST,
			MT_DELETE,
			MT_HEAD,
		};

		e_method		_method;
		std::string		_target;
		std::string		_version;

		std::string										_line;
		std::unordered_map<std::string, std::string>	_headers;
		std::string										_body;

		enum e_status_code
		{
			SC_OK = 200,
		};

	public:

		HttpRequest(void);
		~HttpRequest(void);

		e_method			getMethod(void);
		std::string			getTarget(void);
		std::string			getVersion(void);

		std::unordered_map<std::string, std::string>	parseHeaders(void);
};

#endif
