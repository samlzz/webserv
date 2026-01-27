/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IBodySource.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 14:51:29 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 10:56:40 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IBODY_SOURCE_HPP__
# define __IBODY_SOURCE_HPP__

# include <cstddef>

class IBodySource {

public:
	virtual ~IBodySource() {}

	virtual bool	hasMore(void) const = 0;
	virtual bool	terminated(void) const = 0;

	virtual size_t	read(char *dst, size_t max) = 0;

	virtual void	finalize(void) = 0;
};

#endif /* __IBODY_SOURCE_HPP__ */
