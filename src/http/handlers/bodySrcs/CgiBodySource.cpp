/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiBodySource.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 16:07:39 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 17:46:45 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiBodySource.hpp"
#include "http/cgi/CgiOutputParser.hpp"
#include "http/cgi/CgiProcess.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

CgiBodySource::CgiBodySource(CgiProcess *proc, CgiOutputParser *parser)
	: _parser(parser), _process(proc)
{
	_process->retain();
}

CgiBodySource::~CgiBodySource()
{
	_process->release();
	delete _parser;
}

// ============================================================================
// Methods
// ============================================================================

// ---- IBodySource ----

bool CgiBodySource::hasMore(void) const
{
	if (_parser->bodyHasData())
		return true;

	if (!_process->isTerminated())
		return true;

	return false;
}

bool CgiBodySource::hasError(void) const
{
	return _process->isError();
}

bool CgiBodySource::terminated() const
{
	return _process->isTerminated() && _parser->eof();
}

size_t CgiBodySource::read(char* dst, size_t max)
{
	if (!dst || max == 0)
		return 0;

	if (_parser->bodyHasData())
		return _parser->bodyRead(dst, max);

	return 0;
}

// ---- IMetaSource ----

bool CgiBodySource::metaReady() const
{
	return _parser->metaReady();
}

http::e_status_code CgiBodySource::status() const
{
	return _parser->status();
}

const http::t_headers& CgiBodySource::headers() const
{
	return _parser->headers();
}

// ---- INeedsNotifier ----

void	CgiBodySource::setNotifier(IWritableNotifier &notifier)
{
	_process->setDataNotify(&notifier);
}