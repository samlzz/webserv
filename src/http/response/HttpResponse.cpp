/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 13:30:32 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/30 13:09:23 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <sstream>

#include "HttpResponse.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/response/interfaces/IMetaSource.hpp"
#include "http/routing/Router.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

HttpResponse::HttpResponse(const ResponsePlan &plan, const routing::Context &route)
	: _route(route)
	, _status(plan.status), _headers(plan.headers), _body(plan.body)
	, _out()
	, _commited(false), _done(false)
{}

HttpResponse::~HttpResponse()
{
	delete _body;
}

// ============================================================================
// Accessors
// ============================================================================

IFifoStreamView<t_bytes>&	HttpResponse::stream(void)		{ return _out; }
bool						HttpResponse::isDone() const	{ return _done; }

bool						HttpResponse::shouldCloseConnection(void) const
{
	// TODO: check internal state to determine keep-alive/close
	return false;
}

// ============================================================================
// Methods
// ============================================================================

int32_t	HttpResponse::fillStream(void)
{
	int32_t	added = 0;

	if (!_commited)
	{
		if (!fillMeta(dynamic_cast<IMetaSource *>(_body)))
			return 0;
		commitMeta();
		++added;
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
			return -1;
		}
		char	buffer[RESP_BUFFER_SIZE];
		while (_out.size() < RESP_MAX_BUF_COUNT && _body->hasMore())
		{
			size_t	n = _body->read(buffer, RESP_BUFFER_SIZE);
			if (n == 0)
				break; // ? body not ready now

			_out.push(buffer, n);
			++added;
		}

		if (!_body->hasMore() && _body->terminated())
			_done = true;
	}
	return added;
}

bool	HttpResponse::fillMeta(IMetaSource *meta)
{
	if (!meta)
		return true;

	if (_body->hasError())
	{
		ResponsePlan	err = ErrorBuilder::build(meta->status(), _route.location);
		_status = err.status;
		_headers = err.headers;
		delete _body;
		_body = err.body;
		return true;
	}

	if (!meta->metaReady())
		return false;
	_status = meta->status();
	_headers.insert(
		meta->headers().begin(), meta->headers().end()
	);
	return true;
}

void	HttpResponse::commitMeta(void)
{
	std::ostringstream	oss;

	oss << "HTTP/1.1 " << _status << " " << http::Data::getStatusType(_status)
		<< "\r\n";

	for (http::t_headers::const_iterator it = _headers.begin();
			it != _headers.end(); ++it)
		oss << it->first << ": " << it->second << "\r\n";

	oss << "\r\n";
	_out.push(oss.str());
}