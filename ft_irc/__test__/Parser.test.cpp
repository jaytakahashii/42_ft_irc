/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shonakam <shonakam@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 12:27:04 by shonakam          #+#    #+#             */
/*   Updated: 2025/04/20 21:06:05 by shonakam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cassert>
#include "../include/Parser.hpp" 
#include "../include/color.hpp"

void    testCommand(
	const std::string& message,
	Command expectedCmd,
	const std::vector<std::string>& expectedArgs
) {
	try {
		Parser parser;
		commandS result = parser.parseCommand(message);
		std::cout << "Parsed command: " << result.cmd << std::endl;
		std::cout << "Parsed arguments: ";
		for (size_t i = 0; i < result.args.size(); ++i) {
			std::cout << "[" << result.args[i] << "] ";
		}
		std::cout << std::endl;
		
		assert(result.cmd == expectedCmd);
		assert(result.args == expectedArgs);
		std::cout << "Test passed for message: " << message << std::endl;
	} catch (const Parser::InvalidCommandException& e) {
		std::cerr << "Test failed for message: " << message << " with error: " << e.what() << std::endl;
	}
}

void testJoin(void) {
    // 正常系：引数あり
    std::vector<std::string> args1;
    args1.push_back("#channel");
    testCommand("JOIN #channel", JOIN, args1);

    // 失敗例：引数なし
    std::vector<std::string> args2;
    args2.push_back(""); // 引数なし
    testCommand("JOIN", JOIN, args2);

    // 失敗例：引数が多すぎる
    std::vector<std::string> args3;
    args3.push_back("#channel");
	args3.push_back("#channel2");
    testCommand("JOIN #channel #channel2", JOIN, args3); // 失敗：引数が多すぎる
}

void testPart(void) {
    std::vector<std::string> args1;
    args1.push_back("#channel");
    testCommand("PART #channel", PART, args1);

    // 失敗：引数なし
    std::vector<std::string> args2;
    args2.push_back(""); // 引数なし
    testCommand("PART", PART, args2);

    // 失敗：引数多すぎ
    std::vector<std::string> args3;
    args3.push_back("#channel");
	args3.push_back("extra"); // 余分な引数
    testCommand("PART #channel extra", PART, args3); // 失敗：引数多すぎ
}


void testPrivmsg(void) {
	// 正常系：引数あり
    std::vector<std::string> args1;
    args1.push_back("#channel");
    args1.push_back("Hello");
    testCommand("PRIVMSG #channel :Hello", PRIVMSG, args1);

    // 失敗例：引数が多すぎ
    std::vector<std::string> args2;
    args2.push_back("#channel");
    args2.push_back("Hello");
    args2.push_back("there");
    testCommand("PRIVMSG #channel Hello there", PRIVMSG, args2);

    // 失敗例：引数なし
    std::vector<std::string> args3;
    args3.push_back(""); // 空の引数
    testCommand("PRIVMSG", PRIVMSG, args3);

    // 失敗例：メッセージがない
    std::vector<std::string> args4;
    args4.push_back("#channel");
    testCommand("PRIVMSG #channel", PRIVMSG, args4);
}

void testNick(void) {
    // 正常系：引数あり
    std::vector<std::string> args1;
    args1.push_back("Cyanea");
    testCommand("NICK Cyanea", NICK, args1);

    // 失敗：引数なし
    std::vector<std::string> args2;
    args2.push_back(""); // 引数なし
    testCommand("NICK", NICK, args2);

    // 失敗：引数多すぎ
    std::vector<std::string> args3;
    args3.push_back("user"); // 引数が1つだけ
	args3.push_back("extra"); // 余分な引数
    testCommand("NICK user extra", NICK, args3); // 失敗：引数多すぎ
}


void testUser(void) {
	std::vector<std::string> args1;
	args1.push_back("user");
	args1.push_back("0");
	args1.push_back("*");
	args1.push_back("Real Name");
	testCommand("USER user 0 * :Real Name", USER, args1);

	std::vector<std::string> args2;
	args2.push_back("user");
	args2.push_back("0");
	args2.push_back("*");
	testCommand("USER user 0 *", USER, args2); // 失敗：引数足りない
}

void testPing(void) {
	testCommand("PING", PING, std::vector<std::string>(1, ""));

	std::vector<std::string> args1;
	args1.push_back("token");
	testCommand("PING token", PING, args1); // 失敗：引数がある
}

void testPong(void) {
	std::vector<std::string> args1;
	args1.push_back("abc123");
	testCommand("PONG abc123", PONG, args1);
	
	testCommand("PONG", PONG, std::vector<std::string>(1, "")); // 失敗：引数なし
}

void testQuit(void) {
	testCommand("QUIT", QUIT, std::vector<std::string>(1, ""));
	
	std::vector<std::string> args1;
	args1.push_back("Goodbye");
	testCommand("QUIT :Goodbye", QUIT, args1);

	// std::vector<std::string> args2;
	// args2.push_back("goodbye");
	// testCommand("QUIT goodbye extra", QUIT, args2); // 失敗：引数多すぎ
}

void testKick(void) {
	std::vector<std::string> args1;
	args1.push_back("#channel");
	args1.push_back("user");
	testCommand("KICK #channel user", KICK, args1);

	std::vector<std::string> args2;
	args2.push_back("#channel");
	args2.push_back("user");
	args2.push_back("reason");
	testCommand("KICK #channel user :reason", KICK, args2);

	std::vector<std::string> args3;
	args3.push_back("#channel");
	testCommand("KICK #channel", KICK, args3); // 失敗：引数足りない
}

void testInvite(void) {
	std::vector<std::string> args1;
	args1.push_back("user");
	args1.push_back("#channel");
	testCommand("INVITE user #channel", INVITE, args1);

	std::vector<std::string> args2;
	args2.push_back("user");
	testCommand("INVITE user", INVITE, args2); // 失敗：引数足りない
}

void testTopic(void) {
	std::vector<std::string> args1;
	args1.push_back("#channel");
	testCommand("TOPIC #channel", TOPIC, args1);

	std::vector<std::string> args2;
	args2.push_back("#channel");
	args2.push_back("New Topic");
	testCommand("TOPIC #channel :New Topic", TOPIC, args2);

	testCommand("TOPIC", TOPIC, std::vector<std::string>(1, "")); // 失敗：引数なし
}

void testMode(void) {
    std::vector<std::string> args1;
    args1.push_back("#chan");
    args1.push_back("+I");
    testCommand("MODE #chan +I", MODE, args1);

    std::vector<std::string> args2;
    args2.push_back("#chan");
    args2.push_back("+T");
    testCommand("MODE #chan +T", MODE, args2);

    std::vector<std::string> args3;
    args3.push_back("#chan");
    args3.push_back("+K");
    args3.push_back("secret");
    testCommand("MODE #chan +K secret", MODE, args3);

    std::vector<std::string> args4;
    args4.push_back("#chan");
    args4.push_back("+O");
    args4.push_back("user");
    testCommand("MODE #chan +O user", MODE, args4);

    std::vector<std::string> args5;
    args5.push_back("#chan");
    args5.push_back("+L");
    args5.push_back("user");
    testCommand("MODE #chan +L user", MODE, args5);

    std::vector<std::string> args6;
    args6.push_back("#chan");
    args6.push_back("+X");
    testCommand("MODE #chan +X", MODE, args6); // 失敗：未知のモード

    std::vector<std::string> args7;
    args7.push_back(""); // 引数なし
    testCommand("MODE", MODE, args7); // 失敗：引数なし

    std::vector<std::string> args8;
    args8.push_back("#chan");
    args8.push_back("+K");
    testCommand("MODE #chan +K", MODE, args8); // 失敗：パスワードなし
}

int main() {
	testJoin();
	testPart();
	testPrivmsg();
	testNick();
	testUser();
	testPing();
	testPong();
	testQuit();
	testKick();
	testInvite();
	testTopic();
	testMode();
}
