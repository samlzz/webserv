/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpTransaction.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 11:24:59 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/20 21:01:23 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTP_TRANSACTION_HPP__
# define __HTTP_TRANSACTION_HPP__

#include "config/Config.hpp"
# include "http/request/Cookies.hpp"
# include "http/request/HttpRequest.hpp"
# include "http/response/ResponsePlan.hpp"
# include "http/routing/Router.hpp"
# include "server/AddrInfo.hpp"
# include "server/ServerCtx.hpp"
# include "utils/Optionnal.hpp"

class HttpTransaction {

private:
	Optionnal<routing::Context>	_route;
	ServerCtx					&_ctx;
	const AddrInfo				_remote;
	const AddrInfo				_local;

public:

	// ========================================================================
	// Construction / Destruction
	// ========================================================================
	HttpTransaction(ServerCtx &serv,
					const AddrInfo &remote,
					const AddrInfo &local
				);
	~HttpTransaction();

	// ========================================================================
	// Methods
	// ========================================================================

	ResponsePlan			onParsingError(const HttpRequest &req);
	Optionnal<ResponsePlan>	onHeadersComplete(HttpRequest &req);
	ResponsePlan			onBodyComplete(const HttpRequest &req) const;

	void					reset(void);

	// ========================================================================
	// Accessors
	// ========================================================================

	const Config::Server::Location	*
							getLocation(void) const;
	bool					isHeadersValidated(void) const;

private:
	void					handleSession(Cookies &store);
	void					setCookies(Cookies &store,
										const std::string &query) const;

	// ? Forbidden
	HttpTransaction();
	HttpTransaction(const HttpTransaction &other);
	HttpTransaction& operator=(const HttpTransaction &other);
};


#endif /* __HTTP_TRANSACTION_HPP__ */
