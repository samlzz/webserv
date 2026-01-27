/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHttpHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 14:53:58 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 10:58:46 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IHTTP_HANDLER_HPP__
# define __IHTTP_HANDLER_HPP__

// ============================================================================
// Forward declarations
// ============================================================================

struct ResponsePlan;
class HttpRequest;
namespace routing { struct Context; }

// ============================================================================
// Http Handlers interface
// ============================================================================

class IHttpHandler {

public:
	virtual ~IHttpHandler() {}

	virtual ResponsePlan	handle(
								const HttpRequest &req,
								const routing::Context &route) = 0;
};

#endif /* __IHTTP_HANDLER_HPP__ */
