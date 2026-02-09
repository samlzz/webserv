
#ifndef __COOKIES_HPP__
#define __COOKIES_HPP__


#include "http/HttpTypes.hpp"
#include <string>

class Cookies
{
private:
	http::t_cookies	_cookies;

public:
	Cookies();
	~Cookies();

	void			parseCookies(const http::t_headers &cookieHeader);
	void			parseCookies(const std::string &cookieHeader);

	std::string 	getCookie(const std::string &key) const;
	void 			setCookie(const std::string &key, const std::string &value);

	// std::string 	buildCookieHeader(void);
	// std::string 	buildMultipleCookieHeader(void);
	std::string		buildSetCookieHeaders(void) const;

	void 			print_cookies() const;
};

#endif /* __COOKIES_HPP__ */