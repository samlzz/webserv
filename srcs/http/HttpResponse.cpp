/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achu <achu@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 23:32:00 by achu              #+#    #+#             */
/*   Updated: 2025/11/14 18:54:04 by achu             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

#define CURRENT_STATE() _state
#define UPDATE_STATE(S) _state = S

//#****************************************************************************#
//#                        CONSTRUCTOR & DESTRUCTOR                            #
//#****************************************************************************#

HttpResponse::HttpResponse(void)
{
	
}

HttpResponse::~HttpResponse(void)
{
	
}

//#****************************************************************************#
//#                             STATIC FUNCTION                                #
//#****************************************************************************#



//#****************************************************************************#
//#                             MEMBER FUNCTION                                #
//#****************************************************************************#

void		HttpResponse::handleGET(void)
{
	struct stat	st;

	if (stat(_path.c_str(), &st) < 0)
		return ;// thorw 404

	if (S_ISDIR(st.st_mode)) {
		if (_path.back() != '/')
			return ; // 301 redirect by adding the missing '/'
		
		DIR				*dir;
		struct dirent	*dp;

		std::vector<std::string>		entries;

		if ((dir = opendir(_path.c_str())) == NULL)
			return ;// 404

		while ((dp = readdir(dir)) != NULL) {
			std::string		name = dp->d_name;
			if (dp->d_type == DT_REG) {
				if (name == "." || name == "..")
					continue;
				if (name == "index.html") {
					// do things i guess
				}
				entries.push_back(name);
			}
			else if (dp->d_type == DT_DIR) {
				entries.push_back(name + '/');
			}
		}

		// if autoindex disabled
		// return 403

		// send all the files and dir form vector files

		closedir(dir);
	}
	else if (S_ISREG(st.st_mode)) {
		int	fd = open(_path.c_str(), O_RDONLY);
		if (fd < 0)
			return ; // throw 500

		char	buf[2048];
		int		rb;

		while ((rb = read(fd, buf, sizeof(buf))) > 0) {
			_body.append(buf);
		}

		close(fd);
	}
}

void		HttpResponse::handleHEAD(void)
{
	
}

void		HttpResponse::handlePOST(void)
{
	
}

void		HttpResponse::handlePUT(void)
{
	
}

void		HttpResponse::handleDELETE(void)
{
	
}

void		HttpResponse::build(const http::e_method& pMethod, const std::string& pPath)
{
	switch (_method)
	{
	case http::MTH_GET:
		handleGET();
		break;
	
	default:
		break;
	}
}

//#****************************************************************************#
//#                             GETTER & SETTER                                #
//#****************************************************************************#
