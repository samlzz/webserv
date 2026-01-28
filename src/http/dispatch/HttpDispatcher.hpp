/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpDispatcher.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 11:19:30 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 12:25:08 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTPDISPATCHER_HPP__
# define __HTTPDISPATCHER_HPP__

# include "http/handlers/DeleteHandler.hpp"
# include "http/handlers/ErrorHandler.hpp"
# include "http/handlers/FormHandler.hpp"
# include "http/handlers/RedirectionHandler.hpp"
# include "http/handlers/StaticFileHandler.hpp"
# include "http/handlers/UploadFileHandler.hpp"
# include "http/request/HttpRequest.hpp"
# include "http/response/ResponsePlan.hpp"
# include "http/routing/Router.hpp"

class HttpDispatcher {

private:
	// handlers
	RedirectionHandler	_redirectHandler;
	StaticFileHandler	_staticHandler;
	UploadFileHandler	_uploadHandler;
	FormHandler			_formHandler;
	DeleteHandler		_deleteHandler;
	ErrorHandler		_errHandler;
	// TODO: CgiHandler

public:
	HttpDispatcher();
	~HttpDispatcher();

	ResponsePlan	dispatch(const HttpRequest &req, const routing::Context &route);

private:
	//forbidden
	HttpDispatcher(const HttpDispatcher &other);
	HttpDispatcher& operator=(const HttpDispatcher &other);
};

#endif /* __HTTPDISPATCHER_HPP__ */