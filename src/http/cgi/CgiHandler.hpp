/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:07:20 by achu              #+#    #+#             */
/*   Updated: 2026/01/12 17:13:10 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_HANDLER_HPP__
#define __CGI_HANDLER_HPP__

#include "http/request/HttpRequest.hpp"
#include <vector>
#include <string>

class CgiHandler
{
private:

	http::e_method		_method;
	std::string			_path;
	std::string			_query;
	std::string			_header;
	std::string			_body;
	
	std::vector<std::string>	_env;
	
public:

	CgiHandler(void);
	CgiHandler(HttpRequest pRequest);
	~CgiHandler(void);

	void		runCGI();
};


#endif