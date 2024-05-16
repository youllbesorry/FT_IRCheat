# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bfaure <bfaure@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/06 13:54:50 by bfaure            #+#    #+#              #
#    Updated: 2024/05/14 17:05:27 by bfaure           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

############## VARIABLES ##############

NAME = ircserv

CC = c++ -std=c++98

############## FLAGS ##############

DEPS_FLAGS = -MMD -MP

CFLAGS = ${DEPS_FLAGS} -Wall -Wextra -Werror -g3 -fsanitize=address

############## SOURCES ##############

SRC 	=	srcs/main.cpp \
			srcs/Clients.cpp \
			srcs/Server.cpp \
			srcs/cmd/Join.cpp \
			srcs/cmd/Kick.cpp \
			srcs/Utils.cpp \
			srcs/Channels.cpp


############## DIRECTORIES ##############

DIR_OBJS = .objs

############## OBJECTS ##############

OBJS = $(addprefix $(DIR_OBJS)/, $(SRC:.cpp=.o))

DEPS = ${SRC:%.cpp=${DIR_OBJS}/%.d}

############## RULES ##############

${NAME}	: ${OBJS}
			${CC} ${CFLAGS} -I ./Includes -o $@ $^

-include ${DEPS}

${DIR_OBJS}/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	${CC} ${CFLAGS} -I ./Includes -o $@ -c $<

all 	: ${NAME}

clean	:
			rm -rf ${DIR_OBJS}

fclean	: clean
			rm -f ${NAME}

re		: fclean
			$(MAKE)

############## PHONY ##############

.PHONY: all clean fclean re