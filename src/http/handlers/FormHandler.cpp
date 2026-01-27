#include "http/handlers/FormHandler.hpp"
#include "http/response/ResponsePlan.hpp"
#include "http/request/HttpRequest.hpp"
#include "http/routing/Router.hpp"
#include "config/validation/configValidate.hpp"
#include "bodySrcs/MemoryBodySource.hpp"

#include <sstream>
#include <string>

ResponsePlan	FormHandler::handle(
								const HttpRequest &req,
								const routing::Context &route)
{
	(void)req;
	(void)route;
	ResponsePlan	plan;

	// std::string		convertStr(vec.data(), vec.size());

	std::istringstream stream(req.getBody());
	std::string line;
	std::map<std::string, std::string> data;
	while (std::getline(stream, line, '&'))
	{
		size_t pos = line.find('=');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			data[key] = value;
		}
	}

	std::string	body = "<html><body><ul>\n";
	for (std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); it++)
	{
		body += "<li><b>" + it->first + "</b>:" + it->second + "</li>\n";
	}
	body += "</ul></body></html>\n";

	plan.headers["Content-Type"] = "text/html";
	plan.headers["Content-Length"] = toString(body.size());

	plan.body = new MemoryBodySource(body);

	plan.status = http::SC_OK;
	return  (plan);
}