
#ifndef __SESSIONS_HPP__
#define __SESSIONS_HPP__

#include "http/HttpTypes.hpp"
#include <ctime>
#include <string>
#include <map>

class SessionsManager
{
private:
	struct Session
	{
		std::string							username;
		time_t								last_activity;
		std::map<std::string, std::string>	infos;
	};
	std::map<std::string, Session>	_sessions;

public:
	SessionsManager();
	~SessionsManager();

	std::string 	generateSessionId(void);
	std::string		createSession(const std::string &username);

	Session 		getSession(const std::string &sessionId);

	std::string 	getSessionId(const http::t_headers &headers);
	std::string 	getSessionId(const std::string &field);

	Session 		findSession(const http::t_headers &headers);

	void 			clearExpiredSessions(time_t timeout);
};

#endif /* __SESSIONS_HPP__ */