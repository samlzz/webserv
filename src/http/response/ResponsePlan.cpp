#include "http/response/ResponsePlan.hpp"
#include <ctime>


// ============================================================================
// Private methods
// ============================================================================

void ResponsePlan::addStandardHeaders()
{
	if (this->headers.find("Server") == this->headers.end())
	{
		this->headers["Server"] = "Webserv/1.1";
	}

	if (this->headers.find("Date") == this->headers.end()) {
		char buf[100];
		std::time_t now = std::time(0);
		std::tm *tm = std::gmtime(&now);
		std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm);
		this->headers["Date"] = std::string(buf);
	}
}
