/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IWritableNotifier.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 21:04:47 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/13 21:05:39 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IWRITABLENOTIFIER_HPP__
# define __IWRITABLENOTIFIER_HPP__

class IWritableNotifier {
public:
	virtual ~IWritableNotifier() {}
	virtual void	notifyWritable(void) = 0;
};

#endif /* __IWRITABLENOTIFIER_HPP__ */