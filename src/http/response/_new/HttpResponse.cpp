/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 13:30:32 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 13:55:10 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "http/HttpData.hpp"
#include "http/HttpTypes.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/response/interfaces/IMetaSource.hpp"
#include <sstream>

// ============================================================================
// Construction / Destruction
// ============================================================================

HttpResponse::HttpResponse(const ResponsePlan &plan)
	: _status(plan.status), _headers(plan.headers), _body(plan.body)
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

void	HttpResponse::fillStream(void)
{
	if (!_commited)
	{
	// ? push meta (status and headers)
		IMetaSource	*meta = dynamic_cast<IMetaSource *>(_body);
		if (meta)
		{
			if (!meta->metaReady()) // ? meta not ready now
				return;
			_status = meta->status();
			_headers.insert(
				meta->headers().begin(), meta->headers().end()
			);
		}
		commitMeta();
		_commited = true;
		if (!_body)
			_done = true;
	}

	if (_body)
	{
	// ? push body
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