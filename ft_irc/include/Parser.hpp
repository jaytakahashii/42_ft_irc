/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shonakam <shonakam@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 11:03:53 by shonakam          #+#    #+#             */
/*   Updated: 2025/04/20 20:33:01 by shonakam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>
# include <vector>
# include <set>
# include <sstream>
# include <exception>

/**
 * https://tex2e.github.io/rfc-translater/html/rfc2813.html ref[3.3 Messages]
 * [prefix(option)] [command] [command parameter(up to 15)]
 * 
 * in	: [:username!~user@host PRIVMSG #channel :Hello, World!]
 * out	: ParsedResult
 */

enum Command {
	JOIN,
	PART,
	PRIVMSG,
	NICK,
	USER,
	PING,
	PONG,
	QUIT,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	// MODE_I,
	// MODE_T,
	// MODE_K,
	// MODE_O,
	// MODE_L,
	INVALID
};

struct commandS {
	std::string					prefix;
	Command						cmd;
	std::vector<std::string>	args;
};

class Parser {
	private:	
		bool						isValidCommand(const std::string& cmd);
		bool						isValidArgLength(const std::vector<std::string>& args, const Command cmd);

		void						setCommand(std::istringstream& iss, commandS& result);
		void						setMode(std::istringstream& iss, commandS& result);
		void						setPrefix(std::istringstream& iss, commandS& result);
		void						setArguments(std::string msg, commandS& result);

	public:
		// == OCCF ========================================
		Parser();
		Parser(const Parser& other);
		Parser &operator=(const Parser& other);
		~Parser();

		commandS				parseCommand(const std::string& message);

		class InvalidCommandException : public std::exception {
			private:
				std::string			_msg;
			public :
				explicit InvalidCommandException(const std::string& msg = "Invalid command");
				virtual ~InvalidCommandException() throw();
				virtual const char	*what() const throw();
		};
};
