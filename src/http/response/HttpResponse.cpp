/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 13:30:32 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/18 13:11:09 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ostream>
#include <sstream>
#include <stdint.h>

#include "ft_log/LogOp.hpp"
#include "ft_log/level.hpp"
#include "http/HttpData.hpp"
#include "log.h"
#include "server/ServerCtx.hpp"
#include "HttpResponse.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/response/interfaces/IMetaSource.hpp"
#include "http/routing/Router.hpp"
#include "utils/urlUtils.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

HttpResponse::HttpResponse(const ResponsePlan &plan,
							const HttpRequest &req,
							const routing::Context &route)
	: _ctx(req, route)
	, _status(plan.status), _headers(plan.headers), _body(plan.body)
	, _out()
	, _commited(false), _done(false)
{
	ft_log::log(WS_LOG_HTTP, ft_log::LOG_DEBUG)
		<< "Response plan: status=" << plan.status
		<< ", headers_count=" << plan.headers.size()
		<< ", has_body=" << (plan.body != NULL) << std::endl;
	std::ostream &os = ft_log::log(WS_LOG_HTTP, ft_log::LOG_TRACE) << "Response plan Headers:\n";
	for (http::t_headers::const_iterator it = plan.headers.begin(); it != plan.headers.end(); ++it)
		os << "  " << it->first << ": " << it->second << std::endl;
}

HttpResponse::~HttpResponse()
{
	delete _body;
}

// ============================================================================
// Accessors
// ============================================================================

http::e_status_code			HttpResponse::getStatus(void) const	{ return _status; }
bool						HttpResponse::hasBody(void) const	{ return _body != 0; }

IFifoStreamView<t_bytes>&	HttpResponse::stream(void)			{ return _out; }
bool						HttpResponse::isDone() const		{ return _done; }

/**
 * Create the first buffer from meta and return it
 */
std::string	HttpResponse::rawMeta(void) const
{
	std::ostringstream	oss;

	oss << "HTTP/" << _ctx.request.getVerMaj() << '.' << _ctx.request.getVerMin()
		<< _status << " " << http::Data::getStatusType(_status)
		<< "\r\n";

	for (http::t_headers::const_iterator it = _headers.begin();
			it != _headers.end(); ++it)
		oss << it->first << ": " << it->second << "\r\n";

	oss << "\r\n";
	return oss.str();
}

// ============================================================================
// Public method
// ============================================================================

/**
 * Fille output stream with available content

 * Start by adding meta (status code and headers) if they was available
 * Then push body (if present) while stream size is less than RESP_MAX_BUF_COUNT

 * If no data available (either meta if not commited or body if present)
 *  true must be returned to mean some data will be available later

 * Return:
 *   - true if no error occurs, 0 to n buffer can be pushed in stream
 *   - false otherwise
 */
bool	HttpResponse::fillStream(void)
{
	if (!_commited)
	{
		if (!fillMeta(dynamic_cast<IMetaSource *>(_body)))
			return true;
		_out.push(rawMeta());
		_commited = true;
		if (!_body)
			_done = true;
	}

	if (_body)
	{
	// ? push body
		if (_body->hasError())
		{
			_done = true;
			return false;
		}
		char	buffer[RESP_BUFFER_SIZE];
		while (_out.size() < RESP_MAX_BUF_COUNT && _body->hasMore())
		{
			size_t	n = _body->read(buffer, RESP_BUFFER_SIZE);
			if (n == 0)
				break; // ? body not ready now

			_out.push(buffer, n);
		}

		if (!_body->hasMore() && _body->terminated())
			_done = true;
	}
	return true;
}

bool	HttpResponse::shouldCloseConnection(void) const
{
	http::t_headers::const_iterator	it = _headers.find("Connection");

	if (it != _headers.end() && it->second == "close")
		return true;

	return false;
}

// ============================================================================
// Private methods
// ============================================================================

void	HttpResponse::applyPlan(const ResponsePlan &plan)
{
	_status = plan.status;
	_headers = plan.headers;
	delete _body;
	_body = plan.body;
}

// ---- Meta helpers ----

/**
 * CGI internal redirection is handled as a response-level semantic.

 * Return:
 *   - true if is an internal redirection (so header souldn't be modified)
 *   - false otherwise
*/
bool	HttpResponse::handleCgiRedirect(const std::string &redirectPath)
{
	if (url::isInternal(redirectPath))
	{
		HttpRequest fakeReq(_ctx.request);
		fakeReq.setMethod(http::MTH_GET);
		fakeReq.setPath(redirectPath);

		routing::Context	route = routing::resolve(fakeReq, _ctx.route.server);
		applyPlan(_ctx.route.server.dispatcher.dispatch(fakeReq, route));
		return true;
	}
	else
	{
		delete _body;
		_body = 0;
		return false;
	}
}

/**
 * Fill status code and headers if meta was provided

 * Return:
 *   - true if meta are ready to be commited
 *   - false otherwise, we need to wait until meta are available
*/
bool	HttpResponse::fillMeta(IMetaSource *meta)
{
	if (!meta)
		return true;

	if (_body->hasError())
	{
		applyPlan(ErrorBuilder::build(
								meta->status(),
								_ctx.route.location
							));
		return true;
	}
	if (!meta->metaReady())
		return false;

	http::t_headers::const_iterator	it = meta->headers().find("Location");
	if (
		it != meta->headers().end()
		&& handleCgiRedirect(it->second)
	)
		return true;

	_status = meta->status();
	_headers.insert(
		meta->headers().begin(), meta->headers().end()
	);
	return true;
}

std::ostream	&operator<<(std::ostream &os, const HttpResponse &resp)
{
	os << resp.rawMeta();
	os << (resp.hasBody() ? "With body" : "No body") << std::endl;
	return os;
}