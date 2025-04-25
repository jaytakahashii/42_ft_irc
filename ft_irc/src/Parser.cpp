/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shonakam <shonakam@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 11:40:49 by shonakam          #+#    #+#             */
/*   Updated: 2025/04/20 20:56:11 by shonakam         ###   ########.fr       */
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
 * 1. Use `istringstream` to tokenize the message.
 * 2. If the message starts with ':', treat it as a prefix and skip it.
 * 3. Read the next token as the IRC command.
 * 4. Convert the command to uppercase (IRC is case-insensitive).
 * 5. Return the corresponding Command enum if matched.
 * 6. If no match is found, return Command::INVALID.
 * 7. If the command token is missing, throw `InvalidCommandException`.
 * 8. Check the argument length using `isValidArgLength`. If invalid, throw `InvalidCommandException`.
 * 9. If the command requires further processing (e.g., `MODE`), handle it appropriately.
 * 10. Return the `ParsedResult` containing the parsed command and its arguments.
 */

 commandS					Parser::parseCommand(const std::string& message) {
	std::istringstream	iss(message);
	std::string			token;
	commandS			result;

	setPrefix(iss, result);
	setCommand(iss, result);
	// if (result.cmd == INVALID) throw InvalidCommandException("Invalid command");
	setArguments(message, result);
	// if (!isValidArgLength(result.args, result.cmd)) {
	// 	throw InvalidCommandException("Invalid argument length for command: " + message);
	// }
	return result;
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
		commands.insert("KICK");
		commands.insert("INVITE");
		commands.insert("TOPIC");
		commands.insert("MODE");
	}
	return commands.find(cmd) != commands.end();
}

// bool	Parser::isValidArgLength(const std::vector<std::string>& args, const Command cmd) {
// 	switch (cmd) {
// 		case JOIN:
// 			return args.size() == 1;
// 		case PART:
// 			return args.size() == 1;
// 		case PRIVMSG:
// 			return args.size() >= 2;
// 		case NICK:
// 			return args.size() == 1;
// 		case USER:
// 			return args.size() == 4;
// 		case PING:
// 			return args.size() == 0;
// 		case PONG:
// 			return args.size() == 1;
// 		case QUIT:
// 			return args.size() <= 1;
// 		case KICK:
// 			return args.size() == 2 || args.size() == 3;
// 		case INVITE:
// 			return args.size() == 2;
// 		case TOPIC:
// 			return args.size() == 1 || args.size() == 2;
// 		default:
// 			return false;
// 	}
// }

void Parser::setPrefix(std::istringstream& iss, commandS& result) {
	std::string token;
	if (!(iss >> token)) throw InvalidCommandException("Missing prefix or command");

	if (token[0] == ':') {
		result.prefix = token.substr(1);
	} else {
		iss.clear();
		iss.seekg(0);
		result.prefix = "";
	}
}


void	Parser::setCommand(std::istringstream& iss, commandS& result) {
	std::string			token;

	if (!(iss >> token)) throw InvalidCommandException();

	for (size_t i = 0; i < token.length(); ++i) {
		if (token[i] >= 'a' && token[i] <= 'z')
			token[i] = token[i] - 'a' + 'A';
	}

	if (!isValidCommand(token)) throw InvalidCommandException();

	if (token == "JOIN")		{ result.cmd = JOIN; return ;}
	if (token == "PART")		{ result.cmd = PART; return ;}
	if (token == "PRIVMSG")		{ result.cmd = PRIVMSG; return ;}
	if (token == "NICK")		{ result.cmd = NICK; return ;}
	if (token == "USER")		{ result.cmd = USER; return ;}
	if (token == "PING")		{ result.cmd = PING; return ;}
	if (token == "PONG")		{ result.cmd = PONG; return ;}
	if (token == "QUIT")		{ result.cmd = QUIT; return ;}
	if (token == "KICK")		{ result.cmd = KICK; return ;}
	if (token == "INVITE")		{ result.cmd = INVITE; return ;}
	if (token == "TOPIC")		{ result.cmd = TOPIC; return ;}
	if (token == "MODE") 		{ result.cmd = MODE; return ;}
	result.cmd = INVALID;
}

/*
 * @brief Set the mode for the channel.
 * MODE #channel +I
 * MODE #channel +T
 * MODE #channel +K password
 * MODE #channel +O user
 * MODE #channel +L user
 */
// void	Parser::setMode(std::istringstream& iss, commandS& result) {
// 	std::string channel, mode, extra;
// 	if (!(iss >> channel)) throw InvalidCommandException("Missing channel");
// 	if (!(iss >> mode)) throw InvalidCommandException("Missing mode argument");

// 	result.modeInfo.channel = channel;

// 	if (mode == "+I") {
// 		if (iss >> extra) throw InvalidCommandException("Too many arguments for +I mode");
// 		result.cmd = MODE_I;
// 		return ;
// 	}
// 	if (mode == "+T") { 
// 		if (iss >> extra) throw InvalidCommandException("Too many arguments for +T mode");
// 		result.cmd = MODE_T; 
// 		return ; 
// 	}
// 	if (mode == "+K") {
// 		if (!(iss >> result.modeInfo.password)) throw InvalidCommandException("Missing password");
// 		if (iss >> extra) throw InvalidCommandException("Too many arguments for +K mode");
// 		result.cmd = MODE_K;
// 		return ;
// 	}
// 	if (mode == "+O") {
// 		if (!(iss >> result.modeInfo.user)) throw InvalidCommandException("Missing user");
// 		if (iss >> extra) throw InvalidCommandException("Too many arguments for +O mode");
// 		result.cmd = MODE_O;
// 		return ;
// 	}
// 	if (mode == "+L") {
// 		if (!(iss >> result.modeInfo.user)) throw InvalidCommandException("Missing user");
// 		if (iss >> extra) throw InvalidCommandException("Too many arguments for +L mode");
// 		result.cmd = MODE_L;
// 		return ;
// 	}
// 	throw InvalidCommandException("Unknown mode: " + mode);
// }

#include <iostream>
void	Parser::setArguments(std::string message, commandS& result) {
	std::string	token;
	size_t		position = 0;

	while (position < message.length() && message[position] != ' ') position++; // Skip the command
	while (position < message.length() && message[position] == ' ') position++; // Skip spaces

	while (position < message.length()) {
		if (message[position] == ':') {
			position++;
			while (message[position])
				token += message[position++];
			result.args.push_back(token);
			token.clear();
			break ;
		}
		else if (message[position] == ' ') {
			result.args.push_back(token);
			token.clear();
			position++;
			continue ;
		}
		token += message[position++];
	}
	if (!token.empty()) {
		result.args.push_back(token);
		token.clear();
	}
	if (result.args.size() == 0) {
		result.args.push_back("");
	}
}


// == EXCEPTION ===============================================================
Parser::InvalidCommandException::InvalidCommandException(const std::string& msg): _msg(msg) {}
Parser::InvalidCommandException::~InvalidCommandException() throw() {}
const char*	Parser::InvalidCommandException::what() const throw() {
	return _msg.c_str();
}
