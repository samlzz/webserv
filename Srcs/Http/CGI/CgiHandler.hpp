/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:07:20 by achu              #+#    #+#             */
/*   Updated: 2025/12/10 16:20:26 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_HANDLER_HPP__
#define __CGI_HANDLER_HPP__

#include "../Request/HttpRequest.hpp"
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