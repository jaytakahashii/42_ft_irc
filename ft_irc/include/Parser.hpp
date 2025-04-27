/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shonakam <shonakam@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 11:03:53 by shonakam          #+#    #+#             */
/*   Updated: 2025/04/27 01:08:13 by shonakam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include <set>
#include <sstream>

#define MAX_ARGS	15
#define MAX_BYTE	512

/**
 * https://tex2e.github.io/rfc-translater/html/rfc2813.html ref[3.3 Messages]
 * [prefix(option)] [command] [command parameter(up to 15)]
 * 
 * in	: [:username!~user@host PRIVMSG #channel :Hello, World!]
 * out	: ParsedResult
 */

struct commandS {
	std::string					prefix;
	std::string					name;
	std::vector<std::string>	args;
};

class Parser {
	private:
		bool		_hasPrefix;
		std::string	handleBytes(const std::string& message);
		void		toIrcCharacters(std::string& message);
		void		setCommand(std::istringstream& iss, commandS& result);
		void		setPrefix(std::istringstream& iss, commandS& result);
		void		setArguments(std::string msg, commandS& result);

	public:
		// == OCCF ========================================
		Parser();
		Parser(const Parser& other);
		Parser &operator=(const Parser& other);
		~Parser();

		commandS	parseCommand(const std::string& message);
};
