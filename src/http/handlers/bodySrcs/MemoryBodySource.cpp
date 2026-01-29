/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MemoryBodySource.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 14:22:47 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 18:02:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <unistd.h>

#include "MemoryBodySource.hpp"

// ============================================================================
// Construction / Destruction
// ============================================================================

MemoryBodySource::MemoryBodySource(const std::string &data): _data(data), _offset(0)
{}

MemoryBodySource::~MemoryBodySource()
{}

// ============================================================================
// Methods
// ============================================================================

bool	MemoryBodySource::terminated(void) const
{
	return true;
}

bool	MemoryBodySource::hasMore(void) const
{
	return _offset < _data.size();
}

bool MemoryBodySource::hasError(void) const
{
	return false;
}

size_t	MemoryBodySource::read(char *dst, size_t max)
{
	if (!dst || !max || !hasMore())
		return 0;

	size_t n = _data.size() - _offset;
	if (n > max)
		n = max;

	std::memcpy(dst, _data.data(), n);
	_offset += n;
	return n;
}