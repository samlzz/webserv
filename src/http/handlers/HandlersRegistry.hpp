/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlersRegistry.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:18:30 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 20:30:20 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HANDLERS_REGISTRY_HPP__
# define __HANDLERS_REGISTRY_HPP__

# include <map>

# include "http/handlers/IHttpHandler.hpp"

enum e_handler_kind
{
	HK_STATIC,
	HK_CGI,
	HK_UPLOAD,
	HK_DELETE,
	HK_REDIRECT,
	HK_ERROR
};

class HandlersRegistry {

private:
	typedef std::map<e_handler_kind, const IHttpHandler *>	t_handlers;

	t_handlers	_registry;

public:
	HandlersRegistry();
	~HandlersRegistry();

	void				registerHandler(
							e_handler_kind key,
							const IHttpHandler* handler
						);
	const IHttpHandler*	getHandler(e_handler_kind key) const;

private:
	// forbidden
	HandlersRegistry(const HandlersRegistry &other);
	HandlersRegistry& operator=(const HandlersRegistry &other);
};

#endif /* __HANDLERS_REGISTRY_HPP__ */
