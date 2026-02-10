#include "Cookies.hpp"
#include "http/HttpTypes.hpp"
#include "utils/stringUtils.hpp"

#include <sstream>
#include <iostream>
#include <vector>

Cookies::Cookies()
{}

Cookies::~Cookies()
{}

void 			Cookies::parseCookies(const std::string &cookieHeader)
{
	std::istringstream	stream(cookieHeader);
	std::string			cookiePair;

	while (std::getline(stream, cookiePair, ';'))
	{
		size_t	equalPos = cookiePair.find('=');
		if (equalPos != std::string::npos)
		{
			std::string	key = str::trim(cookiePair.substr(0, equalPos));
			std::string	value = str::trim(cookiePair.substr(equalPos + 1));
	
			// key.erase(0, key.find_first_not_of(" \t"));
			// key.erase(key.find_last_not_of(" \t") + 1);
			// value.erase(0, value.find_first_not_of(" \t"));
			// value.erase(value.find_last_not_of(" \t") + 1);

			_cookies[key] = value;
		}
		else
		{
			std::string	key = str::trim(cookiePair);
			// key.erase(0, key.find_first_not_of(" \t"));
			// key.erase(key.find_last_not_of(" \t") + 1);
			_cookies[key] = "";
		}
	}
}

void 			Cookies::parseCookies(const http::t_headers &cookieHeader)
{
	http::t_headers::const_iterator it = cookieHeader.find("Cookie");
	if (it != cookieHeader.end())
	{
		parseCookies(it->second);
	}
}

std::string 	Cookies::getCookie(const std::string &key) const
{
	http::t_cookies::const_iterator it = _cookies.find(key);
	if (it != _cookies.end())
		return it->second;
	return "";
}

void 			Cookies::setCookie(const std::string &key, const std::string &value)
{
	_cookies[key] = value;
}

// std::string 	Cookies::buildCookieHeader(void)
// {
// 	std::ostringstream	cookieStream;
// 	for (t_cookies::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)
// 	{
// 		cookieStream << it->first << "=" << it->second << "; ";
// 	}
// 	return cookieStream.str();
// }

// std::string Cookies::buildMultipleCookieHeader(void)
// {
// 	std::ostringstream	cookieStream;
// 	for (t_cookies::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)
// 	{
// 		if (it == _cookies.begin())
// 		{
// 			cookieStream << it->first << "=" << it->second << "; ";
// 		}
// 		else
// 		{
// 			cookieStream << "\r\nSet-Cookie: " << it->first << "=" << it->second << "; ";
// 		}
// 	}
// 	return cookieStream.str();
// }

std::string Cookies::buildSetCookieHeaders(void) const
{
	std::vector<std::string> headers;
	std::string allCookies;
	
	for (http::t_cookies::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)
	{
		std::ostringstream cookieStream;
		cookieStream << it->first << "=" << it->second << "; Path=/; HttpOnly";
		headers.push_back(cookieStream.str());
	}
	if (!headers.empty())
	{
		
		for (size_t i = 0; i < headers.size(); ++i)
		{
			if (i > 0)
				allCookies += "\r\nSet-Cookie: ";
			allCookies += headers[i];
		}
	}
	// allCookies += "\r\n";
	return allCookies;
}

void Cookies::print_cookies() const
{
	for (http::t_cookies::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)
	{
		std::cout << "Cookie: " << it->first << " = " << it->second << std::endl;
	}
}