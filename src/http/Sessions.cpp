#include "http/Sessions.hpp"


#include <cstdlib>
#include <ctime>

struct Session;

SessionsManager::SessionsManager()
{
}

SessionsManager::~SessionsManager()
{
}

std::string SessionsManager::generateSessionId(void)
{
	const char charset[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	const size_t max_index = (sizeof(charset) - 1);
	std::string sessionId;
	for (size_t i = 0; i < 16; ++i)
	{
		sessionId += charset[rand() % max_index];
	}
	return sessionId;
}

std::string SessionsManager::createSession(const std::string &username)
{
	std::string sessionId = generateSessionId();
	Session session;
	session.username = username;
	session.last_activity = std::time(0);
	_sessions[sessionId] = session;
	return sessionId;
}
SessionsManager::Session &SessionsManager::getSession(const std::string &sessionId)
{
	return _sessions[sessionId];
}

std::string 		SessionsManager::getSessionId(const http::t_headers &headers) const
{
	http::t_headers::const_iterator it = headers.find("Cookie");
	if (it == headers.end())
		return "";
	return getSessionId(it->second);
}

std::string 		SessionsManager::getSessionId(const std::string &field) const
{
	size_t start = field.find("sessionId=");
	if (start == std::string::npos)
		return "";
	start += 10; // length of "sessionId="
	size_t end = field.find(';', start);
	if (end == std::string::npos)
		end = field.length();
	return field.substr(start, end - start);
}

SessionsManager::Session SessionsManager::findSession(const http::t_headers &headers)
{
	std::string sessionId = getSessionId(headers);
	if (sessionId.empty())
	{
		sessionId = createSession("username");
	}
	return getSession(sessionId);
}

bool SessionsManager::sessionExists(const std::string &sessionId) const
{
	return _sessions.find(sessionId) != _sessions.end();
}

void SessionsManager::clearExpiredSessions(time_t timeout)
{
	for (std::map<std::string, Session>::iterator it = _sessions.begin(); it != _sessions.end();)
	{
		if (std::time(0) - it->second.last_activity > timeout)
		{
			std::map<std::string, Session>::iterator toErase = it;
			++it;
			_sessions.erase(toErase);
		}
		else
		{
			++it;
		}
	}
}