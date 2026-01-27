/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileBodySource.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 14:26:39 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 18:04:18 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FILEBODYSOURCE_HPP__
# define __FILEBODYSOURCE_HPP__

# include <cstddef>

# include "http/response/interfaces/IBodySource.hpp"

class FileBodySource: public IBodySource {

private:
	int			_fd;
	bool		_done;
	bool		_error;

public:
	FileBodySource(int fd);
	virtual ~FileBodySource();

	virtual bool	hasMore(void) const;
	virtual bool	hasError(void) const;
	virtual bool	terminated(void) const;

	virtual size_t	read(char *dest, size_t max);

private:
	// forbidden
	FileBodySource();
	FileBodySource(const FileBodySource &other);
	FileBodySource& operator=(const FileBodySource &other);

};

#endif /* __FILEBODYSOURCE_HPP__ */
