/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutputParser.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 15:25:30 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 16:16:15 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <cstddef>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "CgiOutputParser.hpp"
#include "http/HttpTypes.hpp"
#include "http/response/BuffStream.hpp"
#include "utils/stringUtils.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

CgiOutputParser::CgiOutputParser()
	: _state(ST_HEADERS)
	, _headBuf()
	, _bodyStream()
	, _status(http::SC_OK)
	, _headers()
{}

CgiOutputParser::~CgiOutputParser()
{}

// ============================================================================
// IOutputSink
// ============================================================================

void CgiOutputParser::append(const char* buf, size_t n)
{
	if (_state == ST_DONE || !buf || !n)
		return;

	if (_state == ST_HEADERS)
	{
		_headBuf.append(buf, n);
		if (tryParseHeaders())
			return;
	}
	else if (_state == ST_BODY)
		_bodyStream.push(buf, n);
}

void CgiOutputParser::finalize(void)
{
	_state = ST_DONE;
}

// ============================================================================
// IMetaSource
// ============================================================================

bool CgiOutputParser::metaReady() const
{
	return _state != ST_HEADERS;
}

http::e_status_code CgiOutputParser::status() const
{
	return _status;
}

const http::t_headers& CgiOutputParser::headers() const
{
	return _headers;
}

// ============================================================================
// Body access
// ============================================================================

bool CgiOutputParser::bodyHasData() const
{
	return _bodyStream.hasBuffer();
}

size_t CgiOutputParser::bodyRead(char* dst, size_t max)
{
	if (!dst || max == 0 || !_bodyStream.hasBuffer())
		return 0;

	t_bytes &front = _bodyStream.front();
	size_t n = front.size();
	if (n > max)
		n = max;

	std::memcpy(dst, front.data(), n);

	if (n == front.size())
		_bodyStream.pop();
	else
		front.erase(front.begin(), front.begin() + n);

	return n;
}

bool CgiOutputParser::eof() const
{
	return _state == ST_DONE && !_bodyStream.hasBuffer();
}

// ============================================================================
// Header parsing
// ============================================================================

bool CgiOutputParser::tryParseHeaders()
{
	size_t pos = std::string::npos;
	size_t sepLen = 0;

	if ((pos = _headBuf.find("\r\n\r\n")) != std::string::npos)
		sepLen = 4;
	else if ((pos = _headBuf.find("\n\n")) != std::string::npos) // ? fallback to \n\n
		sepLen = 2;
	else
		return false;

	std::string headerBlock = _headBuf.substr(0, pos);
	parseHeaderLines(headerBlock);

	if (_headers.find("Location") != _headers.end())
		_status = http::SC_FOUND;
	if (_headers.find("Content-Length") == _headers.end())
		_headers["Transfert-Encoding"] = "chunked";

	_state = ST_BODY;

	size_t bodyStart = pos + sepLen;
	if (bodyStart < _headBuf.size())
		_bodyStream.push(_headBuf.substr(bodyStart));
	_headBuf.clear();
	return true;
}

void CgiOutputParser::parseHeaderLines(const std::string& headerBlock)
{
	std::istringstream iss(headerBlock);
	std::string line;

	while (std::getline(iss, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		if (line.empty())
			continue;

		size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue;

		std::string key = str::trim(line.substr(0, colon));
		std::string val = str::trim(line.substr(colon + 1));

		size_t sep = line.find('-');
		if (sep != std::string::npos && sep + 1 < key.size())
			key[sep + 1] = std::toupper(key[sep + 1]);
		if (key == "Status")
		{
			int code = std::atoi(val.c_str());
			if (code >= 100 && code <= 599)
				_status = static_cast<http::e_status_code>(code);
		}
		else
			_headers[key] = val;
	}
}

