# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: shonakam <shonakam@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/09 03:21:52 by shonakam          #+#    #+#              #
#    Updated: 2025/04/20 12:59:20 by shonakam         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := irc_test

INC_DIR := ../include
SRCS_DIR := ../src
OBJS_DIR := objs
OBJS := $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)

parser := $(SRCS_DIR)/Parser.cpp Parser.test.cpp

test_parser:
	@$(CXX) $(CXXFLAGS) -o test_parser $(parser)
	@./test_parser; rm -f test_parser

.PHONY: parser
