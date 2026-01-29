/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:04:11 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 16:56:32 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_HANDLER_HPP__
# define __CGI_HANDLER_HPP__

# include "http/handlers/IHttpHandler.hpp"
# include "http/request/HttpRequest.hpp"
# include "http/response/ResponsePlan.hpp"
# include "http/routing/Router.hpp"

class CgiHandler : public IHttpHandler {
public:
	CgiHandler();
	virtual ~CgiHandler();

	virtual ResponsePlan handle(
							const HttpRequest &req,
							const routing::Context &ctx) const;
};

#endif /* __CGI_HANDLER_HPP__ */