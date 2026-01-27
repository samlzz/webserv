/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MemoryBodySource.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 14:19:23 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 18:02:10 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __MEMORY_BODYSOURCE_HPP__
# define __MEMORY_BODYSOURCE_HPP__

# include <string>
# include <cstddef>

# include "http/response/interfaces/IBodySource.hpp"

class MemoryBodySource : IBodySource {

private:
	std::string	_data;
	size_t		_offset;

public:
	MemoryBodySource(const std::string &data);
	virtual ~MemoryBodySource();

	virtual bool	hasMore(void) const;
	virtual bool	hasError(void) const;
	virtual bool	terminated(void) const;

	virtual size_t	read(char *dest, size_t max);

private:
	//forbidden
	MemoryBodySource();
	MemoryBodySource(const MemoryBodySource &other);
	MemoryBodySource& operator=(const MemoryBodySource &other);

};

#endif /* __MEMORY_BODYSOURCE_HPP__ */
