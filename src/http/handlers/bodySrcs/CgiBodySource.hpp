/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiBodySource.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 15:06:00 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 17:42:03 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGI_BODYSOURCE_HPP__
# define __CGI_BODYSOURCE_HPP__

# include <cstddef>
# include <unistd.h>

# include "http/cgi/CgiOutputParser.hpp"
# include "http/cgi/CgiProcess.hpp"
# include "http/cgi/INeedsNotifier.hpp"
# include "http/response/interfaces/IBodySource.hpp"
# include "http/response/interfaces/IMetaSource.hpp"

class CgiBodySource
	: public IBodySource
	, public IMetaSource
	, public INeedsNotifier {
private:
	CgiOutputParser	*_parser; // owned
	CgiProcess		*_process; // observed

public:
	CgiBodySource(CgiProcess *proc, CgiOutputParser *parser);
	virtual ~CgiBodySource();

	// IBodySource
	virtual bool					hasMore(void) const;
	virtual bool					hasError(void) const;
	virtual bool					terminated(void) const;

	virtual size_t					read(char* dst, size_t max);

	// IMetaSource
	virtual bool					metaReady() const;
	virtual http::e_status_code		status() const;
	virtual const http::t_headers&	headers() const;

	// INeedsNotifier
	virtual void					setNotifier(IWritableNotifier &notifier);

private:
	// forbidden#
	CgiBodySource(const CgiBodySource &other);
	CgiBodySource& operator=(const CgiBodySource &other);
};


#endif /* __CGI_BODYSOURCE_HPP__ */