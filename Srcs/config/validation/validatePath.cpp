/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validatePath.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 11:30:57 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/29 06:38:56 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "configValidate.hpp"

namespace config_validate
{

// -----------------------------------------------------------------------------
// Check filesystem path existence & executability
// -----------------------------------------------------------------------------
static inline bool	pathExists(const std::string &p)
{
	struct stat st;
	return (stat(p.c_str(), &st) == 0);
}

static inline bool	pathIsDir(const std::string &p)
{
	struct stat st;
	if (stat(p.c_str(), &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
}

static inline bool	pathIsFile(const std::string &p)
{
	struct stat st;
	if (stat(p.c_str(), &st) != 0)
		return false;
	return S_ISREG(st.st_mode);
}

static inline bool	pathIsExecutable(const std::string &p)
{
	return (access(p.c_str(), X_OK) == 0);
}

void	validatePathGeneric(
	const std::string &path,
	int32_t rules,
	const std::string &what
)
{
	if (path.empty())
		throw ValueValidationError(what + " cannot be empty");

	if ((rules & PR_MUST_START_SL) && path[0] != '/')
		throw ValueValidationError(what + " must start with '/'", path);

	if ((rules & PR_NO_SLASH) && path.find('/') != std::string::npos)
		throw ValueValidationError(what + " contains forbidden '/'", path);

	if ((rules & PR_NO_DOTDOT) && path.find("..") != std::string::npos)
		throw ValueValidationError(what + " contains forbidden '..'", path);

	if (rules & PR_NO_REGEX)
	{
		for (size_t i = 0; i < path.size(); ++i)
		{
			switch (path[i])
			{
				case '*': case '?': case '[': case ']':
					throw ValueValidationError(
						what + " contains invalid characters", path);
			}
		}
	}

	if ((rules & PR_MUST_EXIST) && !pathExists(path))
		throw ValueValidationError(what + " does not exist", path);

	if ((rules & PR_MUST_DIR) && !pathIsDir(path))
		throw ValueValidationError(what + " is not a directory", path);

	if ((rules & PR_MUST_FILE) && !pathIsFile(path))
		throw ValueValidationError(what + " is not a file", path);

	if ((rules & PR_MUST_EXEC) && !pathIsExecutable(path))
		throw ValueValidationError(what + " is not executable", path);
}

} // namespace config_validate