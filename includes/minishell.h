#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <stdio.h>
# include <fcntl.h>
# include <signal.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>

# define STDIN 0
# define STDOUT 1
# define STDERR 2

# define SUCCESS 0
# define ERROR 1
# define SYNTAX_ERROR 258
# define UNKNOWN_COMMAND 127

typedef struct s_env
{
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_data
{
	// t_token	*tokens;
	t_env	*env;
	// t_cmd	*commands;
	// pid_t	*fam;
	int	cmd_count;
	int	status;
}	t_data;

extern t_data	*g_data;

void	sig_init(void);
void	sig_default(void);
void	sig_ignore(void);
void	sig_heredoc(void);

void	sig_quit(int sig);
void	sig_interrupt(int sig);

#endif