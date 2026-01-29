/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorBuilder.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:50:00 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 16:18:08 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ERROR_HANDLER_HPP__
# define __ERROR_HANDLER_HPP__

# include "config/Config.hpp"
# include "http/HttpTypes.hpp"
# include "http/handlers/IHttpHandler.hpp"
# include "http/response/ResponsePlan.hpp"

class ErrorBuilder {

public:

	static ResponsePlan	build(
							http::e_status_code status,
							const Config::Server::Location *location 
						);
private:

	// ---- Helpers ----
	static ResponsePlan	buildDefault(http::e_status_code status);
	static ResponsePlan	buildFromErrorPage(
							http::e_status_code status,
							const std::string& path
						);

	// class not constructable
	ErrorBuilder();
	~ErrorBuilder();
	ErrorBuilder(const ErrorBuilder &other);
	ErrorBuilder& operator=(const ErrorBuilder &other);
};

#endif /* __ERROR_HANDLER_HPP__ */