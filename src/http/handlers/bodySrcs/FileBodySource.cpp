/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileBodySource.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 14:28:13 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 15:55:01 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "FileBodySource.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

FileBodySource::FileBodySource(int fd)
	: _fd(fd), _done(false), _error(false)
{
	if (_fd < 0)
		_done = true;
}

FileBodySource::~FileBodySource()
{
	if (_fd > 0)
		close(_fd);
}

// ============================================================================
// Methods
// ============================================================================

bool	FileBodySource::terminated(void) const
{
	return !_error;
}

bool	FileBodySource::hasMore(void) const
{
	return !_done;
}

bool	FileBodySource::hasError(void) const
{
	return _error;
}

size_t	FileBodySource::read(char *dst, size_t max)
{
	if (_done || _fd < 0 || !dst || !max)
		return 0;

	ssize_t n = ::read(_fd, dst, max);

	if (n <= 0)
		_done = true;
	if (n < 0)
	{
		_error = true;
		return 0;
	}

	return n;
}

