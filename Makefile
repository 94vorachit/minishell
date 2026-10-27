NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC = $(wildcard src/*.c) \
	$(wildcard signal/*.c) \
	$(wildcard token/*.c)
OBJ = $(SRC:.c=.o)
LDLIBS = -lreadline

all: $(NAME)

$(NAME): $(OBJ)
	$(MAKE) -C libft
	$(CC) $(CFLAGS) $(OBJ) $(LDLIBS) libft/libft.a -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(MAKE) clean -C ./libft/
	rm -f $(OBJ)

fclean: clean
	$(MAKE) fclean -C ./libft/
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re