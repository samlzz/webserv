/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponsePlan.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 14:48:42 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 10:57:20 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RESPONSE_PLAN_HPP__
# define __RESPONSE_PLAN_HPP__

# include "http/HttpTypes.hpp"

// ============================================================================
// Forward declarations
// ============================================================================

class IBodySource;
struct ConnEvent;

// ============================================================================
// Immutable plan for HttpResponse
// ============================================================================

struct ResponsePlan {

	http::e_status_code	status;
	http::t_headers		headers;
	// may be null:
	IBodySource			*body;
	ConnEvent			*event;

	ResponsePlan(): status(http::SC_OK), headers(), body(0), event(0)
	{}
};

#endif /* __RESPONSE_PLAN_HPP__ */
