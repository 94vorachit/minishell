/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 19:00:13 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 19:03:55 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <stdio.h> //printf, perror, file/stdout
# include <errno.h> //errno
# include <fcntl.h> //open and file flags.
# include <signal.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/types.h> //pid_t
# include <sys/wait.h> //waitpid()
# include <unistd.h> //fork, execve, dup, close, pipe, access, write, read,
# include <stddef.h> //size_t appears in the header structs

# define STDIN 0
# define STDOUT 1
# define STDERR 2

# define SUCCESS 0
# define ERROR 1
# define SYNTAX_ERROR 2
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

typedef struct s_cmd
{
	char			**argv;
	char			*infile;
	char			*outfile;
	char			*heredoc;
	int				heredoc_fd;
	int				append;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_data
{
	t_token		*tokens;
	t_env		*env;	
	t_env		*export;	
	t_cmd		*commands;
	int			status;
}	t_data;

typedef struct s_exp_state
{
	t_data		*data;
	const char	*word;
	size_t		*i;
	char		**result;
	int			in_single;
	int			in_double;
}	t_exp_state;

extern volatile sig_atomic_t	g_signal;

/* src */
int		init(t_data *mini, char **env);
void	free_shell(t_data *data);
void	ft_error(int error_code);
void	ft_shlvl(t_env **env);
int		ft_init_env(t_env **mini_env, char **env);

int		check_args(int ac, char **av);
int		setup_data(t_data **data, char **env);
void	apply_signal_status(t_data *data);
int		check_line_state(t_data *data, char **line);
int		handle_input_line(t_data *data, char **line);

int		execute_pipeline_cmds(t_data *data);
int		run_simple_cmd_from_tokens(t_data *data, t_cmd *cmd);
int		execute_token_stream(t_data *data, t_cmd *cmd, char *line);
int		main(int ac, char **av, char **env);

char	*env_get(t_env *env, const char *name);
t_env	*env_find(t_env *env, char *name);
int		env_set(t_env **env, char *name, char *value);
void	env_unset(t_env **env, char *name);
int		env_count(t_env *env);
char	**sort_env_alpha(t_env *env);
void	ft_env_clear(t_env **env);
t_env	*ft_env_new(char *value);

/* signal */
void	sig_init(void);
void	sig_default(void);
void	sig_ignore(void);
void	sig_heredoc(void);

/* token */
int		ft_isoperator(char c);
int		ft_add_char(char **word, char c);
t_token	*ft_token_new(char *value, t_token_type type);
void	ft_token_add_back(t_token **tokens, t_token *new_token);
void	ft_token_clear(t_token **tokens);
int		ft_isredirection(t_token_type type);
int		ft_add_token(t_token **tokens, char *value, t_token_type type);
int		ft_quote_word(char *line, int *index, char **word);
int		ft_read_word(char *line, int *index, char **word, int *started);
int		ft_lex_word(char *line, int *index, t_token **tokens);
char	*set_pipe_token(char *line, int *index, t_token_type *type);
char	*set_heredoc_token(char *line, int *index, t_token_type *type);
char	*set_append_token(char *line, int *index, t_token_type *type);
char	*set_redir_in_token(char *line, int *index, t_token_type *type);
char	*set_redir_out_token(char *line, int *index, t_token_type *type);
void	ft_print_tokens(t_token *tokens);
int		lexer(char *line, t_token **tokens);
int		ft_validate_syntax(t_token *tokens);
int		word_has_quote(const char *word);
void	remove_empty_word(t_token **tokens, t_token **cur, t_token **pre);

/* expand */
int		is_var_start(char c);
int		is_var_char(char c);
int		is_var_name_char(char c);
char	*append_text(char *result, const char *text);
char	*append_char(char *result, char c);
int		expand_status_var(t_data *data, char **result, size_t *i);
int		expand_env_var(t_data *data, const char *word,
			size_t *i, char **result);
int		handle_dollar(t_data *data, const char *word, size_t *i, char **result);
int		handle_single_quote(int *in_single, size_t *i);
int		handle_double_quote(int *in_double, size_t *i);
int		append_literal_char(char **result, const char *word, size_t *i);
int		process_char(t_exp_state *state);
char	*expand_word(t_data *data, const char *word);
int		expand_tokens(t_data *data, t_token **tokens);

/* exec */
int		parse_redirections(t_token *tokens, t_cmd *cmd);
int		build_simple_cmd(t_token *tokens, t_cmd *cmd);
void	assign_redirection(t_cmd *cmd, t_token_type type, char *value);
int		count_commands(t_cmd *cmd);
void	setup_child_io(t_cmd *cur, int prev_fd, int pipefd[2]);
int		redirect_child_cmd(t_cmd *cur);

void	ft_free_split(char **arr);
char	**env_to_char_array(t_env *env);
void	ft_free_cmd(t_cmd *cmd);
int		ft_count_env(t_env *env);
int		ft_copy_env(char **arr, t_env *env);
char	*find_executable_path(t_env *env, const char *cmd);
void	ft_free_cmd_list(t_cmd **cmds);

int		run_builtin_cmd(t_data *data, t_cmd *cmd);
int		run_external_cmd(t_data *data, t_cmd *cmd);
int		redirect_input(t_cmd *cmd);
int		redirect_output(t_cmd *cmd);
void	run_child(t_cmd *cmd, char *exec_path, char **envp);
void	set_child_status(t_data *data, int status);
int		run_process(t_data *data, t_cmd *cmd, char *exec_path, char **envp);
int		execute_simple_cmd(t_data *data, t_cmd *cmd);
int		execute_pipeline(t_data *data, t_cmd *cmd);
void	update_parent_pipe(int *prev_fd, t_cmd *cur, int pipefd[2]);
void	run_child_cmd(t_data *data, t_cmd *cur, int prev_fd, int pipefd[2]);

int		prepare_execution(t_data *data, t_cmd *cmd,
			char **exec_path, char ***envp);
void	exec_external_child(t_cmd *cmd, char *exec_path, char **envp);

/* pipe */
int		build_pipeline_cmds(t_token *tokens, t_cmd **head);
int		ft_has_pipe(t_token *tokens);

/* builtin */
int		is_builtin(char *cmd);
int		execute_builtin(t_data *data, t_cmd *cmd);
int		builtin_echo(t_data *data, t_cmd *cmd);
int		builtin_cd(t_data *data, t_cmd *cmd);
int		builtin_pwd(t_data *data, t_cmd *cmd);
int		builtin_export(t_data *data, t_cmd *cmd);
int		builtin_unset(t_data *data, t_cmd *cmd);
int		builtin_env(t_data *data, t_cmd *cmd);
int		builtin_exit(t_data *data, t_cmd *cmd);

/* heredoc */
int		prepare_heredoc_cmd(t_cmd *cmd);
int		prepare_heredoc_list(t_cmd *cmds);
int		write_heredoc_to_pipe(char *delim, int fd);

#endif