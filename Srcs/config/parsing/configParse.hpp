/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 09:10:18 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/10 02:30:39 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSEPROPS_HPP__
# define __PARSEPROPS_HPP__

# include <stdexcept>
# include <stdint.h>

# include "ftpp/AstNode.hpp"
# include "config/Config.hpp"

namespace config_parse
{

// ============================================================================
// Errors
// ============================================================================
class WsParseError : public std::runtime_error {
public:
	WsParseError(const std::string &msg): std::runtime_error(msg) {}
};

class PropError : public WsParseError {
public:
	PropError(const std::string &msg): WsParseError(msg) {}
	PropError(const std::string &prop, const std::string &msg)
		: WsParseError(prop + ": " + msg) {}
};

class ValueError : public PropError {
public:
	ValueError(const std::string &what)
		: PropError("Empty " + what + " value") {}
	ValueError(const std::string &what, const std::string &value)
		: PropError("Invalid " + what + " value", "'" + value + "'") {}
};

// ============================================================================
// Extraction
// ============================================================================

Config::Server	extractServer(const AstNode *serverNode);

// O is T or Optionnal<T>
template<typename T, typename O>
void	fillDest(O &dest, const AstNode *node, const std::string &prop,
					T (*extract)(const std::string &))
{
	if (!node->hasAttr(prop))
		return;
	dest = extract(node->getAttr(prop, ""));
}

template<typename O>
void	fillDest(O &dest, const AstNode *node, const std::string &prop)
{
	if (!node->hasAttr(prop))
		return;
	dest = node->getAttr(prop, "");
}

} // namespace config_parse

#endif /* __PARSEPROPS_HPP__ */