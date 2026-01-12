/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 22:43:24 by achu              #+#    #+#             */
/*   Updated: 2026/01/12 00:39:09 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpStatus.hpp"

namespace http {

	static const StatusCode statusData[] = {
		{ 200, "OK" },
		{ 201, "Created" },
		{ 204, "No Content" },
		{ 301, "Moved Permanently" },
		{ 400, "Bad Request" },
		{ 403, "Forbidden" },
		{ 404, "Not Found" },
		{ 500, "Internal Server Error" },
		{ 501, "Not Implemented" },
		{ 0,   NULL }
	};

	StatusCode		getStatusCode(int code)
	{
		for (int i = 0; statusData[i].code != 0; i++) {
			if (statusData[i].code == code)
				return (statusData[i]);
		}

		StatusCode _status;
   		_status.code = 500;
    	_status.reason = "Internal Server Error";
    
    	return (_status);
	}
}