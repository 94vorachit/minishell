# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/22 19:07:48 by vorhansa          #+#    #+#              #
#    Updated: 2026/10/29 18:36:20 by vorhansa         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

SRCS = $(wildcard *.c)
OBJS = ${SRCS:.c=.o}

BONUS = $(wildcard ft_lst*.c) ft_next.c
OBJSBONUS = ${BONUS:.c=.o}

CC		= gcc
RM		= rm -f

CFLAGS = -Wall -Wextra -Werror -g

all:	${NAME}

$(NAME): ${OBJS}
		ar rcs ${NAME} ${OBJS}

bonus:	${OBJS} ${OBJSBONUS}
		ar rcs ${NAME} ${OBJS} ${OBJSBONUS}

clean:
		${RM} ${OBJS} ${OBJSBONUS}

fclean:	clean
		${RM} ${NAME}

re:		fclean all

.PHONY:	all clean fclean re bonus
