/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shonakam <shonakam@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 12:27:04 by shonakam          #+#    #+#             */
/*   Updated: 2025/04/20 13:07:35 by shonakam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../include/Parser.hpp" 
#include "../include/color.hpp"

void testValidCommands(Parser& parser) {
    try {
        std::cout << "Test: Valid Command (PRIVMSG)" << std::endl;
        Command cmd = parser.parseCommand(":username!~user@host PRIVMSG #channel :Hello, World!");
        std::cout << "Result: " << (cmd == PRIVMSG ? "PASS" : "FAIL") << std::endl;

        std::cout << "Test: Valid Command (JOIN)" << std::endl;
        cmd = parser.parseCommand(":username!~user@host JOIN #channel");
        std::cout << "Result: " << (cmd == JOIN ? "PASS" : "FAIL") << std::endl;

        std::cout << "Test: Valid Command (PONG)" << std::endl;
        cmd = parser.parseCommand(":username!~user@host PONG :server");
        std::cout << "Result: " << (cmd == PONG ? "PASS" : "FAIL") << std::endl;

        std::cout << "Test: Valid Command (NICK)" << std::endl;
        cmd = parser.parseCommand(":username!~user@host NICK newnick");
        std::cout << "Result: " << (cmd == NICK ? "PASS" : "FAIL") << std::endl;

        std::cout << "Test: Valid Command (QUIT)" << std::endl;
        cmd = parser.parseCommand(":username!~user@host QUIT :Goodbye");
        std::cout << "Result: " << (cmd == QUIT ? "PASS" : "FAIL") << std::endl;
    } catch (const Parser::InvalidCommandException& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
    }
}

void testInvalidCommands(Parser& parser) {
    try {
        std::cout << "Test: Invalid Command (INVALIDCOMMAND)" << std::endl;
        parser.parseCommand(":username!~user@host INVALIDCOMMAND #channel :message");
        std::cout << "Result: FAIL (Exception expected)" << std::endl;
    } catch (const Parser::InvalidCommandException& e) {
        std::cout << "Result: PASS (Exception caught)" << std::endl;
    }
}

void testPrefixHandling(Parser& parser) {
    try {
        std::cout << "Test: Command with prefix" << std::endl;
        Command cmd = parser.parseCommand(":username!~user@host PRIVMSG #channel :message");
        std::cout << "Result: " << (cmd == PRIVMSG ? "PASS" : "FAIL") << std::endl;
    } catch (const Parser::InvalidCommandException& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
    }
}

void testCaseInsensitivity(Parser& parser) {
    try {
        std::cout << "Test: Command case insensitivity" << std::endl;
        Command cmd = parser.parseCommand(":username!~user@host part #channel");
        std::cout << "Result: " << (cmd == PART ? "PASS" : "FAIL") << std::endl;
    } catch (const Parser::InvalidCommandException& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
    }
}

void testEmptyCommand(Parser& parser) {
    try {
        std::cout << "Test: Empty Command" << std::endl;
        parser.parseCommand(":username!~user@host :NoCommandHere");
        std::cout << "Result: FAIL (Exception expected)" << std::endl;
    } catch (const Parser::InvalidCommandException& e) {
        std::cout << "Result: PASS (Exception caught)" << std::endl;
    }
}

void testWeirdSpacing(Parser& parser) {
    try {
        std::cout << "Test: Command with extra spaces" << std::endl;
        Command cmd = parser.parseCommand("   PRIVMSG     #chan    :hello world");
        std::cout << "Result: " << (cmd == PRIVMSG ? "PASS" : "FAIL") << std::endl;
    } catch (...) {
        std::cout << "Result: FAIL (Unexpected exception)" << std::endl;
    }
}

void testCommandWithCRLF(Parser& parser) {
    try {
        std::cout << "Test: Command with CRLF" << std::endl;
        Command cmd = parser.parseCommand("PING :server\r\n");
        std::cout << "Result: " << (cmd == PING ? "PASS" : "FAIL") << std::endl;
    } catch (...) {
        std::cout << "Result: FAIL (Unexpected exception)" << std::endl;
    }
}

void testEmptyLine(Parser& parser) {
    try {
        std::cout << "Test: Empty Line" << std::endl;
        parser.parseCommand("");
        std::cout << "Result: FAIL (Expected exception)" << std::endl;
    } catch (...) {
        std::cout << "Result: PASS (Exception caught)" << std::endl;
    }
}

static void printMSG(const std::string msg) {
    std::cout   << BG_GREEN
                << "== " << msg << " =="
                << RESET
                << std::endl;
}

/**
 * Main function to run all tests.
 * [ ':' <prefix> <SPACE> ] <command> [ <params> ] <crlf>
 */
int main() {
    Parser parser;

    printMSG("valid command");
    testValidCommands(parser);
    
    printMSG("invalid command");
    testInvalidCommands(parser);

    printMSG("prefix handling");
    testPrefixHandling(parser);

    printMSG("case insensitivity");
    testCaseInsensitivity(parser);

    printMSG("weird spacing");
    testEmptyCommand(parser);
    
    printMSG("weird spacing");
    testWeirdSpacing(parser);

    printMSG("command with CRLF");
    testCommandWithCRLF(parser);

    printMSG("empty line");
    testEmptyLine(parser);
}
