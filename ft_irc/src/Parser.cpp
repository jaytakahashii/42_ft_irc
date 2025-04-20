/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shonakam <shonakam@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 11:40:49 by shonakam          #+#    #+#             */
/*   Updated: 2025/04/20 12:57:50 by shonakam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

// == OCCF ====================================================================
Parser::Parser(void) { return ; }
Parser::Parser(const Parser& other) { *this = other; }
Parser::~Parser(void) { return ; }
Parser& Parser::operator=(const Parser& other) {
	if (this != &other) { }
	return *this;
}

// == PUBLIC ==================================================================

/*
 * @brief Parse the command from the message.
 *
 * Steps:
 * 1. Use istringstream to tokenize the message.
 * 2. If the message starts with ':', treat it as a prefix and skip it.
 * 3. Read the next token as the IRC command.
 * 4. Convert the command to uppercase (IRC is case-insensitive).
 * 5. Return the corresponding Command enum if matched.
 * 6. If no match is found, return Command::INVALID.
 * 7. If the command token is missing, throw InvalidCommandException.
 */
Command						Parser::parseCommand(const std::string& message) {
	std::istringstream	iss(message);
	std::string			token;

	if (!message.empty() && message[0] == ':') iss >> token;
	if (!(iss >> token)) throw InvalidCommandException();

	for (size_t i = 0; i < token.length(); ++i) {
		if (token[i] >= 'a' && token[i] <= 'z')
			token[i] = token[i] - 'a' + 'A';
	}
	
	if (!isValidCommand(token)) throw InvalidCommandException();

	if (token == "JOIN")		return JOIN;
	if (token == "PART")		return PART;
	if (token == "PRIVMSG")		return PRIVMSG;
	if (token == "NICK")		return NICK;
	if (token == "USER")		return USER;
	if (token == "PING")		return PING;
	if (token == "PONG")		return PONG;
	if (token == "QUIT")		return QUIT;
	return INVALID;
}

std::vector<std::string>	parseArguments(const std::string& message) {
	static_cast<void>(message);
	return std::vector<std::string>();
}

std::string					getErrorMessage(Command cmd) {
	static_cast<void>(cmd);
	return std::string("");
}

// == PRIVATE =================================================================


bool	Parser::isValidCommand(const std::string& cmd) {
	static std::set<std::string> commands;
	if (commands.empty()) {
		commands.insert("JOIN");
		commands.insert("PART");
		commands.insert("PRIVMSG");
		commands.insert("NICK");
		commands.insert("USER");
		commands.insert("PING");
		commands.insert("PONG");
		commands.insert("QUIT");
	}
	return commands.find(cmd) != commands.end();
}


// == EXCEPTION ===============================================================
const char*	Parser::InvalidCommandException::what() const throw() {
	return "Invalid command.";
}
