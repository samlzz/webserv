/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IChunkEncoder.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 09:31:51 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/20 13:41:12 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ICHUNK_ENCODER_HPP__
# define __ICHUNK_ENCODER_HPP__

# include <string>

class IChunkEncoder {

public:
	virtual ~IChunkEncoder() {}

	/**
	 * Encode a piece of body data into one or more HTTP buffers
	 */
	virtual void	encode(const char *buf, size_t bufSize) = 0;

	/**
	 * Called once when no more data will be produced
	 */
	virtual void	finalize(void) = 0;
};

#endif /* __ICHUNK_ENCODER_HPP__ */
