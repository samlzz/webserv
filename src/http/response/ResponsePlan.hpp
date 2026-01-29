/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponsePlan.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 14:48:42 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/27 12:56:49 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RESPONSE_PLAN_HPP__
# define __RESPONSE_PLAN_HPP__

# include "http/HttpTypes.hpp"
# include "http/response/interfaces/IBodySource.hpp"
# include "server/connections/ConnEvent.hpp"

// ============================================================================
// Immutable plan for HttpResponse
// ============================================================================

struct ResponsePlan {

	http::e_status_code	status;
	http::t_headers		headers;
	IBodySource			*body; // ? raw pointer, may be null, ownership transfert
	ConnEvent			event; // ? default to CE_NONE

	ResponsePlan()
		: status(http::SC_OK), headers(), body(0), event(ConnEvent::none())
	{}
};

#endif /* __RESPONSE_PLAN_HPP__ */