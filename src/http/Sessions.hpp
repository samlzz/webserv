
#ifndef __SESSIONS_HPP__
#define __SESSIONS_HPP__

#include "http/HttpTypes.hpp"
#include <ctime>
#include <string>
#include <map>

class SessionsManager
{
public:
	struct Session
	{
		std::string							username;
		time_t								last_activity;
		bool 								is_logged_in;
		std::map<std::string, std::string>	infos;

		Session()
			: username("Guest"), last_activity(std::time(0)), is_logged_in(false)
		{}

		~Session() {}

		void 	updateActivity(void) 		{last_activity = std::time(0);}
		bool 	getIsLoggedIn(void) const 	{return is_logged_in;}
		void 	setIsLoggedIn(bool val) 	{is_logged_in = val;}
	};

private:
	std::map<std::string, Session>	_sessions;

public:
	SessionsManager();
	~SessionsManager();

	std::string 	generateSessionId(void);
	std::string		createSession(const std::string &username);

	Session 		&getSession(const std::string &sessionId);
	Session 		&getSession(const std::string &sessionId) const;
	std::string 	getSessionId(const http::t_headers &headers) const;
	std::string 	getSessionId(const std::string &field) const;

	Session 		findSession(const http::t_headers &headers);
	
	bool			sessionExists(const std::string &sessionId) const;

	void 			clearExpiredSessions(time_t timeout);
};

#endif /* __SESSIONS_HPP__ */