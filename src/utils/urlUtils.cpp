#include "urlUtils.hpp"
#include "utils/convertUtils.hpp"

namespace url {

std::string	decode(const std::string& encoded)
{
	std::string	path;
	int32_t		dec;

	for (size_t i = 0; i < encoded.length(); i++)
	{
		if (encoded[i] == '%' && i + 2 < encoded.length())
		{
			dec = convert::htod(encoded.substr(i + 1, 2));
			if (dec > 0)
			{
				path.push_back(static_cast<unsigned char>(dec));
				i+=2;
				continue;
			}
		}
		if (encoded[i] == '+')
			path.push_back(' ');
		else
			path.push_back(encoded[i]);
	}
	return path;
}

bool	isValidEncoded(const std::string& encoded)
{
	for (size_t i = 0; i < encoded.length(); i++)
	{
		if (encoded[i] == '%' && i + 2 < encoded.length())
		{
			if (convert::htod(encoded.substr(i + 1, 2)) < 0)
				return false;
			i++;
		}
	}
	return true;
}

} // namespace url
