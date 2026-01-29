/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpDispatcher.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 11:19:30 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 14:02:52 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_DISPATCHER_HPP__
# define __HTTP_DISPATCHER_HPP__

# include "http/handlers/DeleteHandler.hpp"
# include "http/handlers/FormHandler.hpp"
# include "http/handlers/IHttpHandler.hpp"
# include "http/handlers/RedirectionHandler.hpp"
# include "http/handlers/StaticFileHandler.hpp"
# include "http/handlers/UploadFileHandler.hpp"
# include "http/request/HttpRequest.hpp"
# include "http/response/ResponsePlan.hpp"

namespace routing { struct Context; }

class HttpDispatcher {

private:
	// handlers
	RedirectionHandler	_redirectHandler;
	StaticFileHandler	_staticHandler;
	UploadFileHandler	_uploadHandler;
	FormHandler			_formHandler;
	DeleteHandler		_deleteHandler;
	// TODO: CgiHandler

public:
	HttpDispatcher();
	~HttpDispatcher();

	ResponsePlan	dispatch(
						const HttpRequest &req,
						const routing::Context &route
					) const;

private:
	const IHttpHandler	*findHandler(
						const HttpRequest &req,
						const routing::Context &route
					) const;

	//forbidden
	HttpDispatcher(const HttpDispatcher &other);
	HttpDispatcher& operator=(const HttpDispatcher &other);
};

#endif /* __HTTP_DISPATCHER_HPP__ */