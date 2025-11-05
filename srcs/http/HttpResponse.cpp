/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:00 by achu              #+#    #+#             */
/*   Updated: 2025/11/05 17:39:18 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.h"

#define CURRENT_STATE() _state
#define UPDATE_STATE(S) _state = S

//#****************************************************************************#
//#                        CONSTRUCTOR & DESTRUCTOR                            #
//#****************************************************************************#

HttpResponse::HttpResponse(void)
{
	
}

HttpResponse::~HttpResponse(void)
{
	
}

//#****************************************************************************#
//#                             STATIC FUNCTION                                #
//#****************************************************************************#

//#****************************************************************************#
//#                             MEMBER FUNCTION                                #
//#****************************************************************************#

void		HttpResponse::handleGET(void)
{
	
}

void		HttpResponse::handleHEAD(void)
{
	
}

void		HttpResponse::handlePOST(void)
{
	
}

void		HttpResponse::handlePUT(void)
{
	
}

void		HttpResponse::handleDELETE(void)
{
	
}

void		HttpResponse::build(const http::e_method& pMethod, const std::string& pPath)
{
	switch (CURRENT_STATE())
	{
	case RES_START:
		
		break;
	
	default:
		break;
	}
}

//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#
