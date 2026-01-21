/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 18:58:18 by sliziard          #+#    #+#             */
/*   Updated: 2026/01/21 20:25:10 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_H
# define LOG_H

// ============================================================================
// Separator
// ============================================================================

# ifndef WS_LOG_SEPARATOR
#  define WS_LOG_SEPARATOR		'='
# endif

# ifndef WS_LOG_SEP_WIDTH
#  define WS_LOG_SEP_WIDTH		40
# endif

# define WS_LOG_SEP				std::string(WS_LOG_SEP_WIDTH, WS_LOG_SEPARATOR)

// ============================================================================
// Categories
// ============================================================================

# define WS_LOG					"webserv"
# define WS_LOG_CONFIG			"webserv.config"
# define WS_LOG_SERVER			"webserv.server"
# define WS_LOG_SERVER_CLI		"webserv.server.client"

#endif