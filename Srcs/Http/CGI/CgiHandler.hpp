/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:07:20 by achu              #+#    #+#             */
/*   Updated: 2025/12/03 17:43:34 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_HANDLER_HPP__
#define __CGI_HANDLER_HPP__

#include "../Request/HttpRequest.hpp"

class CgiHandler
{
private:

	HttpRequest		_request;
	
public:

	CgiHandler(HttpRequest pRequest);
	~CgiHandler();

	void		run();
};


#endif