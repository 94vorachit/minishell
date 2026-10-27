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

typedef enum e_token_type
{
    T_WORD,
    T_PIPE,
    T_REDIR_IN,
    T_REDIR_OUT,
    T_APPEND,
    T_HEREDOC
}	t_token_type;

/* For storing information about tokens and quotes */
typedef struct s_token
{
    char			*value;
    t_token_type	type;
    struct s_token	*next;
}	t_token;

/* For storing the environment variables line by line as a node */
typedef struct s_env
{
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_data
{
	t_token	*tokens;
	t_env	*env;
	// t_cmd	*commands;
	// pid_t	*fam;
	int	cmd_count;
	int	status;
}	t_data;

extern t_data	*g_data;

void	ft_env_clear(t_env **env);

void	sig_init(void);
void	sig_default(void);
void	sig_ignore(void);
void	sig_heredoc(void);

void	sig_quit(int sig);
void	sig_interrupt(int sig);

int 	ft_isoperator(char c);
int		ft_add_char(char **word, char c);
t_token	*ft_token_new(char *value, t_token_type type);
void	ft_token_add_back(t_token **tokens, t_token *new_token);
void	ft_token_clear(t_token **tokens);

int		ft_add_token(t_token **tokens, char *value, t_token_type type);
int		ft_quote_word(char *line, int *index, char **word);
int		ft_read_word(char *line, int *index, char **word, int *started);
int		ft_lex_word(char *line, int *index, t_token **tokens);

void	ft_print_tokens(t_token *tokens);
int		lexer(char *line, t_token **tokens);

int		ft_validate_syntax(t_token *tokens);

#endif