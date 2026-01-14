/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IWritableNotifier.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 21:04:47 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/14 13:24:27 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IWRITABLE_NOTIFIER_HPP__
# define __IWRITABLE_NOTIFIER_HPP__

// ============================================================================
// Writable notification interface
// ============================================================================
class IWritableNotifier {
public:

	virtual ~IWritableNotifier() {}

	/**
	 * Notify that new data is available for writing.
	 *
	 * The implementation is expected to update its poll interest
	 * (e.g. enable POLLOUT) so that the Reactor will schedule a write.
	 */
	virtual void	notifyWritable(void) = 0;
};

#endif /* __IWRITABLE_NOTIFIER_HPP__ */