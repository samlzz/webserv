#pragma once

#include "http/request/HttpRequest.hpp"
#include "http/response/HttpResponse.hpp"
#include <string>
#include <map>
#include <ctime>


class Cookie
{
private:
	HttpRequest	*_request;
	HttpResponse	*_response_;

	typedef std::map<std::string, std::string>	t_headers;

	typedef std::map<std::string, std::string>	t_cookies;
	struct SessionData
	{
		std::string		username;
		time_t		last_activity;
		t_cookies	_cookies;
	};

	typedef std::map<std::string, SessionData>	t_id;
	static t_id _sessionIds;

public:
	void		setRequest(HttpRequest &pRequest);
	void		setResponse(HttpResponse *pResponse);

	std::map<std::string, std::string> 		parseCookies(void);
	std::string searchIDInCookies(void);
	std::string generateSessionId(void);
	bool		checkSession(void);
	bool		checkSession(const std::string &pSessionId);
	void		createCookieHeader(const std::string &id);
	void		addCookie(const std::string &id ,const std::string &pKey, const std::string &pValue);

	void		createSession(const std::string &lang);
	std::string buildCookieHeader(void);
	void 		appendCookieHeader(const std::string &pHeader);
	void 		createCookieHeader();
	void 		clearSession(const std::string &pSessionId);
	void 		clearExpiredSessions(time_t pExpirationTime);
	void 		addCookieOptions(const std::string &pOptions);

};