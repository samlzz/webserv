/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:50:00 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 10:55:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ERROR_HANDLER_HPP__
# define __ERROR_HANDLER_HPP__

# include "config/Config.hpp"
# include "http/HttpTypes.hpp"
# include "http/handlers/IHttpHandler.hpp"
# include "http/response/ResponsePlan.hpp"

class ErrorHandler: public IHttpHandler{

public:
	ErrorHandler();

	ResponsePlan			build(
								http::e_status_code status,
								const Config::Server::Location *location = 0
							) const;
	virtual ResponsePlan	handle(
								const HttpRequest &req,
								const routing::Context &route);

private:

	// ---- Helpers ----
	ResponsePlan	buildDefault(http::e_status_code status) const;
	ResponsePlan	buildFromErrorPage(
						http::e_status_code status,
						const std::string& path
					) const;
};

#endif

