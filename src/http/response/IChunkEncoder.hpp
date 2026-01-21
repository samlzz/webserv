/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IChunkEncoder.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 09:31:51 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/21 17:22:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ICHUNK_ENCODER_HPP__
# define __ICHUNK_ENCODER_HPP__

# include <cstddef>

class IChunkEncoder {

public:
	virtual ~IChunkEncoder() {}

	/**
	 * Encode a piece of body data into one or more HTTP buffers
	 */
	virtual void	encode(const char *buffer, std::size_t bufSize) = 0;

	/**
	 * Called once when no more data will be produced
	 */
	virtual void	finalize(void) = 0;
};

#endif /* __ICHUNK_ENCODER_HPP__ */
