#include "http/handlers/FormHandler.hpp"
#include "http/response/BuffStream.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"
#include "bodySrcs/MemoryBodySource.hpp"
#include "utils/stringUtils.hpp"
#include "utils/fileSystemUtils.hpp"

#include <sstream>
#include <string>



ResponsePlan	FormHandler::handle(
								const HttpRequest &req,
								const routing::Context &route) const
{
	(void)req;
	(void)route;
	ResponsePlan	plan;

	const t_bytes		&reqBody = req.getBody();
	std::istringstream	stream(std::string(reqBody.begin(), reqBody.end()));
	std::string			line;
	std::map<std::string, std::string> data;
	while (std::getline(stream, line, '&'))
	{
		size_t pos = line.find('=');
		if (pos != std::string::npos)
		{
			std::string key = fs::url_decode(line.substr(0, pos));
			std::string value = fs::url_decode(line.substr(pos + 1));
			data[key] = value;
		}
	}

	if (route.location->sessionLogin == true && route.session
		&& data.find("username") != data.end()
		&& !data["username"].empty())
	{
		route.session->username = data["username"];
		if (route.session->username == "admin")
			route.session->setIsLoggedIn(true);
		else
			route.session->setIsLoggedIn(false);
	}

	std::string	body = "<html><body><ul>\n";
	for (std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); it++)
	{
		body += "<li><b>" + it->first + "</b>:" + it->second + "</li>\n";
	}
	body += "<a href=\"/\">Back to home</a>\n";
	body += "</ul></body></html>\n";

	plan.headers["Content-Type"] = "text/html";
	plan.headers["Content-Length"] = str::toString(body.size());

	plan.body = new MemoryBodySource(body);

	plan.status = http::SC_OK;
	return  (plan);
}