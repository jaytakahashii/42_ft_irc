MAKEFILE 		= Makefile

NAME				= ircserv

SRC_DIR			= src/
REP_DIR			= numericsReplies/
CMD_DIR			= commands/
UTIL_DIR		= utils/
OBJ_DIR			= .obj/

INC_DIR			= include/
INC					= -I $(INC_DIR)
CMD_H				= PassCommand.hpp \
							NickCommand.hpp \
							UserCommand.hpp \
							JoinCommand.hpp \
							PartCommand.hpp \
							PingCommand.hpp \
							PrivmsgCommand.hpp \
							QuitCommand.hpp \
							KickCommand.hpp \
							TopicCommand.hpp \

UTIL_H			= utils.hpp \
							color.hpp

REP_H				= 001-099.hpp \
							400-499.hpp

HEADERS			= Server.hpp \
							Client.hpp \
							CommandDispatcher.hpp \
							Channel.hpp \
							Parser.hpp

HEADERS			+= $(addprefix $(CMD_DIR), $(CMD_H))
HEADERS			+= $(addprefix $(UTIL_DIR), $(UTIL_H))
HEADERS			+= $(addprefix $(REP_DIR), $(REP_H))
HEADER			+= $(addprefix $(INC_DIR), $(HEADERS))
DEPS				= $(HEADER) $(MAKEFILE)

CXX					= c++
CXXFLAGS		= -Wall -Wextra -Werror -std=c++98
PEDANT			= -pedantic
SANITIZE		= -fsanitize=address -g3

RM					= rm -rf

SRC_DIR			= src/
CMD_SRC			= PassCommand.cpp \
							NickCommand.cpp \
							UserCommand.cpp \
							JoinCommand.cpp \
							PartCommand.cpp \
							PingCommand.cpp \
							PrivmsgCommand.cpp \
							QuitCommand.cpp \
							KickCommand.cpp \
							TopicCommand.cpp

UTIL_SRC		= utils.cpp

SRC_FILES		=	main.cpp \
							Server.cpp \
							Client.cpp \
							CommandDispatcher.cpp \
							Channel.cpp \
							Parser.cpp

SRC_FILES		+= $(addprefix $(CMD_DIR), $(CMD_SRC))
SRC_FILES		+= $(addprefix $(UTIL_DIR), $(UTIL_SRC))
SRCS				= $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJS				= $(addprefix $(OBJ_DIR), $(SRC_FILES:.cpp=.o))

# color ************
Y		= "\033[33m"
R		= "\033[31m"
G		= "\033[32m"
B		= "\033[34m"
X		= "\033[0m"
UP	= "\033[A"
CUT	= "\033[K"
# ******************

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS) $(DEPS)
	@echo $(Y) "src files successfully compiled.\n" $(X)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo $(G) "!! $(NAME) created !!\n" $(X)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)$(CMD_DIR)
	@mkdir -p $(OBJ_DIR)$(UTIL_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)
	@echo $(R) "$(OBJ_DIR) has been removed.\n" $(X)

fclean:
	$(RM) $(OBJ_DIR)
	$(RM) $(NAME)
	@echo $(R) "$(NAME) and $(OBJ_DIR) has been removed.\n" $(X)

re: fclean all

pedant: CXXFLAGS += $(PEDANT)
pedant: re
	@echo $(G) "pedantic mode activated.\n" $(X)

leaks: CXXFLAGS += $(SANITIZE)
leaks: re
	@echo $(G) "leaks mode activated.\n" $(X)

.PHONY: all clean fclean re pedant leaks
