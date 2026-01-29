/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IMetaSource.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 17:34:27 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 10:54:25 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IMETA_SOURCE_HPP__
# define __IMETA_SOURCE_HPP__

# include "http/HttpTypes.hpp"

class IMetaSource {
public:
	virtual ~IMetaSource() {}
	virtual bool					metaReady() const = 0;
	virtual http::e_status_code		status() const = 0;
	virtual const http::t_headers&	headers() const = 0;
};

#endif /* __IMETA_SOURCE_HPP__ */
