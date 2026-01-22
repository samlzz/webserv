#include "http/response/Cookie.hpp"
#include <sstream>

void	Cookie::setRequest(HttpRequest &pRequest)
{
	_request = &pRequest;
}

void	Cookie::setResponse(HttpResponse *pResponse)
{
	_response_ = pResponse;
}
// Parse the "Cookie" header from the request and store cookies in the _cookies map
std::map<std::string, std::string>	Cookie::parseCookies(void)
{
	std::string	cookieHeader = (*_request).getHeaderValue("Cookie");
	
	if (cookieHeader.empty())
		return std::map<std::string, std::string>();
	
	t_cookies cookies;
	std::istringstream	stream(cookieHeader);
	std::string			cookiePair;

	while (std::getline(stream, cookiePair, ';'))
	{
		size_t	equalPos = cookiePair.find('=');
		if (equalPos != std::string::npos)
		{
			std::string	key = cookiePair.substr(0, equalPos);
			std::string	value = cookiePair.substr(equalPos + 1);
	
			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);

			// std::cout << "Cookie: " << key << " = " << value << std::endl;
			cookies[key] = value;
		}
	}
	return cookies;
}
std::string Cookie::searchIDInCookies(void)
{
	std::map<std::string, std::string>	cookies = parseCookies();
	std::map<std::string, std::string>::const_iterator it = cookies.find("sessionId");
	if (it != cookies.end())
		return it->second;
	return "";
}

bool Cookie::checkSession(void)
{
	std::string sessionId = searchIDInCookies();
	if (sessionId.empty())
		return false;
	return checkSession(sessionId);
}

bool Cookie::checkSession(const std::string &pSessionId)
{
	t_id::const_iterator it = _sessionIds.find(pSessionId);
	if (it != _sessionIds.end())
		return true;
	return false;
}

std::string Cookie::generateSessionId(void)
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

void Cookie::createSession(const std::string &username)
{
	std::string sessionId = generateSessionId();
	Cookie::SessionData sessionData;
	sessionData.username = username;
	sessionData.last_activity = std::time(0);
	_sessionIds[sessionId] = sessionData;
	addCookie(sessionId, "sessionId", sessionId);
	_response_->addHeader("Set-Cookie", buildCookieHeader());
}

// Create the "Set-Cookie" header string from the _cookies map
//Set-Cookie=sessionId=abc123; lang=fr
void	Cookie::createCookieHeader(const std::string &id)
{
	std::ostringstream	cookieStream;
	t_id::const_iterator itId = _sessionIds.find(id);
	if (itId == _sessionIds.end())
		return;
	const t_cookies		&_cookies = itId->second._cookies;

	for (t_cookies::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)
	{
		cookieStream << it->first << "=" << it->second << "; ";
	}

	std::string	cookieHeader = cookieStream.str();
	if (!cookieHeader.empty())
		cookieHeader.erase(cookieHeader.length() - 2);

	_response_->addHeader("Set-Cookie", cookieHeader);
}

void Cookie::addCookie(const std::string &id ,const std::string &pKey, const std::string &pValue)
{
	t_id::iterator it = _sessionIds.find(id);
	if (it != _sessionIds.end())
	{
		it->second._cookies[pKey] = pValue;
	}
}

std::string Cookie::buildCookieHeader(void)
{
	std::ostringstream	cookieStream;
	t_id::const_iterator itId = _sessionIds.find(searchIDInCookies());
	if (itId == _sessionIds.end())
		return "";
	const t_cookies		&_cookies = itId->second._cookies;
	for (t_cookies::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)
	{
		cookieStream << it->first << "=" << it->second << "; ";
	}

	std::string	cookieHeader = cookieStream.str();
	if (!cookieHeader.empty())
		cookieHeader.erase(cookieHeader.length() - 2);

	return (cookieHeader);
}

void Cookie::appendCookieHeader(const std::string &pHeader)
{
	std::string existingHeader = "";
	t_headers::iterator it = _response_->getResponse().headers.find("Set-Cookie");
	if (it != _response_->getResponse().headers.end())
		existingHeader = it->second;

	if (!existingHeader.empty())
		existingHeader += "; ";

	existingHeader += pHeader;
	_response_->addHeader("Set-Cookie", existingHeader);
}

void Cookie::createCookieHeader()
{
	std::string sessionId = searchIDInCookies();
	if (sessionId.empty())
		return;
	createCookieHeader(sessionId);
}

void Cookie::clearSession(const std::string &pSessionId)
{
	t_id::iterator it = _sessionIds.find(pSessionId);
	if (it != _sessionIds.end())
		_sessionIds.erase(it);
}

void Cookie::clearExpiredSessions(time_t pExpirationTime)
{
	time_t currentTime = std::time(0);
	for (t_id::iterator it = _sessionIds.begin(); it != _sessionIds.end(); )
	{
		if (currentTime - it->second.last_activity > pExpirationTime)
		{
			t_id::iterator toErase = it;
			++it;
			_sessionIds.erase(toErase);
		}
		else
			++it;
	}
}

void Cookie::addCookieOptions(const std::string &pOptions)
{
	std::string existingHeader = "";
	t_headers::iterator it = (_response_->getResponse().headers).find("Set-Cookie");
	if (it != (_response_->getResponse().headers).end())
		existingHeader = it->second;

	if (!existingHeader.empty())
		existingHeader += "; ";

	existingHeader += pOptions;
	_response_->addHeader("Set-Cookie", existingHeader);
}