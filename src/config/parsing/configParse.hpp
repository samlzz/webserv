/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 09:10:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/22 14:12:03 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CONFIG_PARSE_HPP__
# define __CONFIG_PARSE_HPP__

# include "RawConfig.hpp"
# include "ftpp/AstNode.hpp"
# include "ftpp/FtppException.hpp"

namespace config_parse
{

// ============================================================================
// Errors
// ============================================================================
class WsParseError : public FtppException {
public:
	WsParseError(const std::string &msg): FtppException(msg) {}
};

class ValueError : public WsParseError {
public:
	ValueError(const std::string &what)
		: WsParseError("Empty " + what + " value") {}
	ValueError(const std::string &what, const std::string &value)
		: WsParseError("Invalid " + what + " value" + ": '" + value + "'") {}
};

// ============================================================================
// Extraction
// ============================================================================

RawServer	extractServer(const AstNode *serverNode);

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

#endif /* __CONFIG_PARSE_HPP__ */