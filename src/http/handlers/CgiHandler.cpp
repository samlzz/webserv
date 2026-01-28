/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 16:05:13 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/28 17:43:34 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "http/HttpTypes.hpp"
#include "http/cgi/CgiProcess.hpp"
#include "http/cgi/CgiOutputParser.hpp"
#include "bodySrcs/CgiBodySource.hpp"
#include "http/dispatch/ErrorBuilder.hpp"
#include "http/routing/Router.hpp"
#include "server/connections/ConnEvent.hpp"


CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}

ResponsePlan CgiHandler::handle(
	const HttpRequest& req,
	const routing::Context& route
) const
{
	ResponsePlan plan;

	CgiOutputParser* parser = new CgiOutputParser();
	CgiProcess* process = new CgiProcess(*parser);
	process->retain();

	IConnection* readConn = process->start(
		getScriptPath(req),
		getArgv(req),
		getEnvp(req),
		req.getBody()
	);

	if (!readConn)
	{
		process->release();
		delete parser;

		return ErrorBuilder::build(
				http::SC_INTERNAL_SERVER_ERROR,
				route.location);
	}

	CgiBodySource* body = new CgiBodySource(process, parser);
	process->release();

	plan.status = http::SC_OK;
	plan.body = body;
	plan.event = ConnEvent::spawn(readConn);

	return plan;
}
