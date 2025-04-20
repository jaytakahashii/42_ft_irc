/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shonakam <shonakam@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 11:03:53 by shonakam          #+#    #+#             */
/*   Updated: 2025/04/20 12:19:17 by shonakam         ###   ########.fr       */
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
 * out	: Cmmand(enum)
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
	INVALID
};

class Parser {
	private:
		void	handleJoin(const std::vector<std::string>& args);
		void	handlePart(const std::vector<std::string>& args);
		void	handlePrivmsg(const std::vector<std::string>& args);
		void	handleNick(const std::vector<std::string>& args);
		void	handleUser(const std::vector<std::string>& args);
		void	handlePing(const std::vector<std::string>& args);
		void	handlePong(const std::vector<std::string>& args);
		void	handleQuit(const std::vector<std::string>& args);
	
		bool	isValidCommand(const std::string& cmd);
		bool	isValidNick(const std::string& nick);
		bool	isValidChannel(const std::string& channel);
		bool	isValidMessage(const std::string& message);
		
		bool	isPrefix(const std::string& message);
		std::string	extractPrefix(const std::string& message);

	public:
		Parser();
		Parser(const Parser& other);
		Parser &operator=(const Parser& other);
		~Parser();

		Command						parseCommand(const std::string& message);
		std::vector<std::string>	parseArguments(const std::string& message);
		std::string					getErrorMessage(Command cmd);

		class InvalidCommandException : public std::exception {
			public :
				virtual const char	*what() const throw();
		};
};
