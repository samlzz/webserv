/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutputParser.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 14:50:30 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/29 15:32:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_OUTPUT_PARSER_HPP__
# define __CGI_OUTPUT_PARSER_HPP__

# include <string>

# include "IOutputSink.hpp"
# include "http/HttpTypes.hpp"
# include "http/response/BuffStream.hpp"
# include "http/response/interfaces/IMetaSource.hpp"

class CgiOutputParser : public IMetaSource, public IOutputSink {

private:
	enum e_state { ST_HEADERS, ST_BODY, ST_DONE };
	e_state					_state;

	std::string				_headBuf;
	BuffStream				_bodyStream;

	http::e_status_code		_status;
	http::t_headers			_headers;

public:
	CgiOutputParser();
	virtual ~CgiOutputParser();

	// IOutputSink
	virtual void					append(const char* buf, size_t n);
	virtual void					finalize(void);

	// IMetaSource
	virtual bool					metaReady() const;
	virtual http::e_status_code		status() const;
	virtual const http::t_headers&	headers() const;

	// body access
	bool							bodyHasData() const;
	size_t							bodyRead(char* dst, size_t max);
	bool							eof() const; // EOF CGI atteint

private:

	bool tryParseHeaders(); // ? returns true when headers completed+parsed
	void parseHeaderLines(const std::string& headerBlock);

	// forbidden
	CgiOutputParser(const CgiOutputParser &other);
	CgiOutputParser	&operator=(const CgiOutputParser &outher);
};

#endif