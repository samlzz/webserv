
#ifndef __COOKIES_HPP__
#define __COOKIES_HPP__


#include "http/HttpTypes.hpp"
#include <map>
#include <string>

class Cookies
{
private:
	typedef std::map<std::string, std::string> t_cookies ;
	t_cookies	_cookies;

public:
	Cookies();
	~Cookies();

	void			parseCookies(const http::t_headers &cookieHeader);
	void			parseCookies(const std::string &cookieHeader);

	std::string 	getCookie(const std::string &key) const;
	void 			setCookie(const std::string &key, const std::string &value);
	std::string 	buildCookieHeader(void);
};

#endif /* __COOKIES_HPP__ */