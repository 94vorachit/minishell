/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 19:00:13 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/23 16:16:59 by vorhansa         ###   ########.fr       */
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
# include <sys/stat.h>
# include <unistd.h> //fork, execve, dup, close, pipe, access, write, read,
# include <stddef.h> //size_t appears in the header structs
# include <limits.h>

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
	char			**heredocs;
	int				heredoc_fd;
	int				append;
	int				input_heredoc;
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

/* src/init_utils.c */
int		init(t_data *mini, char **env);
void	ft_free_shell(t_data *data);
void	ft_error(int error_code);
void	ft_shlvl(t_env **env);
int		ft_init_env(t_env **mini_env, char **env);

/* src/init_utils2.c */
int		ft_check_args(int ac, char **av);
int		ft_setup_data(t_data **data, char **env);
void	ft_apply_signal_status(t_data *data);
int		ft_check_line_state(char **line);
int		ft_handle_input_line(t_data *data, char **line);

/* src/env_utils.c */
int		ft_update_env_value(t_env *node, char *name, char *value);
int		ft_env_add_last(t_env **env, t_env *node);
int		ft_env_set(t_env **env, char *name, char *value);
void	ft_env_unset(t_env **env, char *name);
int		ft_env_count(t_env *env);

/* src/env_utils2.c */
t_env	*ft_env_find(t_env *env, char *name);
char	*ft_env_get(t_env *env, const char *name);
void	ft_env_clear(t_env **env);
t_env	*ft_env_new(char *value);

/* src/env_utils3.c */
char	**ft_copy_env_array(t_env *env, int count);
void	ft_sort_env_array(char **arr, int count);
char	**ft_sort_env_alpha(t_env *env);

/* src/main_exec.c */
int		ft_exec_input_chunk(t_data *data, t_cmd *cmd, char *chunk);
int		ft_handle_multiline_input(t_data *data, t_cmd *cmd, char *line);
int		ft_exec_pipe_cmds(t_data *data);
int		ft_run_simple_cmd_from_tokens(t_data *data, t_cmd *cmd);
int		ft_exec_token_stream(t_data *data, t_cmd *cmd, char *line);

/* src/main_exec2.c */
int		ft_count_chunks(char **chunks);
void	ft_mark_heredoc_body(char **chunks, int *skip, int start,
			char *delimiter);
void	ft_exec_heredoc_chunks_first(t_data *data, t_cmd *cmd,
			char **chunks, int *skip);
void	ft_exec_remaining_chunks(t_data *data, t_cmd *cmd,
			char **chunks, int *skip);

/* src/main.c */
int		main(int ac, char **av, char **env);

/* signal/signal.c */
void	ft_sig_reprompt(int sig);
void	ft_sig_init(void);

/* signal/signal2.c */
void	ft_sig_default(void);
void	ft_sig_ignore(void);
void	ft_sig_heredoc(void);

/* token/lexer.c */
void	ft_print_tokens(t_token *tokens);
char	*ft_operator_value(char *line, int *index, t_token_type *type);
int		ft_lex_operator(char *line, int *index, t_token **tokens);
int		ft_lexer(char *line, t_token **tokens);

/* token/syntax.c */
int		ft_check_start_pipe(t_token *tokens);
int		ft_check_pipe_case(t_token *prev, t_token *cur, t_token *next);
int		ft_check_redir_case(t_token *cur, t_token *next);
int		ft_validate_syntax(t_token *tokens);

/* token/syntax2.c */
void	ft_print_syntax_error(t_token *token);
void	ft_print_syntax_error_str(char *value);
void	ft_print_syntax_eof(void);

/* token/token_types.c */
char	*ft_set_pipe_token(char *line, int *index, t_token_type *type);
char	*ft_set_heredoc_token(char *line, int *index, t_token_type *type);
char	*ft_set_append_token(char *line, int *index, t_token_type *type);
char	*ft_set_redir_in_token(char *line, int *index, t_token_type *type);
char	*ft_set_redir_out_token(char *line, int *index, t_token_type *type);

/* token/token_utils.c */
int		ft_quote_word(char *line, int *index, char **word);
int		ft_read_word_char(char *line, int *index, char **word);
int		ft_read_word(char *line, int *index, char **word, int *started);
int		ft_lex_word(char *line, int *index, t_token **tokens);

/* token/token_utils2.c */
int		ft_add_token(t_token **tokens, char *value, t_token_type type);
int		ft_add_char(char **word, char c);
t_token	*ft_token_new(char *value, t_token_type type);
void	ft_token_add_back(t_token **tokens, t_token *new_token);
void	ft_token_clear(t_token **tokens);

/* token/token_utils3.c */
int		ft_isredir(t_token_type type);
int		ft_isoperator(char c);

/* expand/expand_utils.c */
int		ft_is_var_start(char c);
int		ft_is_var_char(char c);
char	*ft_append_text(char *result, const char *text);
char	*ft_append_char(char *result, char c);

/* expand/expand_utils2.c */
int		ft_handle_single_quote(int *in_single, size_t *i);
int		ft_handle_double_quote(int *in_double, size_t *i);
int		ft_append_literal_char(char **result, const char *word, size_t *i);
int		ft_expand_status_var(t_data *data, char **result, size_t *i);
int		ft_word_has_quote(const char *word);

/* expand/expand.c */
int		ft_expand_env_var(t_data *data, const char *word,
			size_t *i, char **result);
int		ft_handle_dollar(t_data *data, const char *word, size_t *i,
			char **result);
int		ft_process_char(t_exp_state *state);
char	*ft_expand_word(t_data *data, const char *word);
void	ft_remove_empty_word(t_token **tokens, t_token **cur, t_token **pre);

/* expand/expand2.c */
int		ft_expand_current_word(t_data *data, t_token **tokens,
			t_token **cur, t_token **pre);
int		ft_expand_tokens(t_data *data, t_token **tokens);

/* exec/exec_parsing.c */
int		ft_count_args(t_token *tokens);
int		ft_fill_argv(t_token *tokens, t_cmd *cmd);
int		ft_init_simple_cmd(t_cmd *cmd);
int		ft_finalize_simple_cmd(t_cmd *cmd, int parse_ret);
int		ft_build_simple_cmd(t_token *tokens, t_cmd *cmd);

/* exec/exec_path.c */
void	ft_free_split(char **arr);
char	*ft_check_exec_in_dir(const char *dir, const char *cmd);
char	*ft_search_in_paths(char **paths, const char *cmd);
char	*ft_find_exec_path(t_env *env, const char *cmd);

/* exec/exec_redir.c */
void	ft_set_input_file(t_cmd *cmd, char *value);
void	ft_set_output_file(t_cmd *cmd, char *value, int append);
void	ft_set_heredoc(t_cmd *cmd, char *value);
void	ft_assign_redir(t_cmd *cmd, t_token_type type, char *value);

/* exec/exec_redir2.c */
int		ft_open_output_redir(char *value, t_token_type type);
int		ft_set_redir(t_cmd *cmd, t_token *token);
int		ft_handle_redir_in(t_cmd *cmd, t_token *cur);
int		ft_handle_redir_token(t_token *cur, int in_pipeline, t_cmd *cmd);
int		ft_parse_redir(t_token *tokens, t_cmd *cmd);

/* exec/exec_redir3.c */
int		ft_has_pipe(t_token *tokens);
int		ft_redir_file(char *infile);
int		ft_redir_input(t_cmd *cmd);
int		ft_open_output_file(t_cmd *cmd);
int		ft_redir_output(t_cmd *cmd);

/* exec/exec_utils.c */
void	ft_free_cmd(t_cmd *cmd);
void	ft_free_cmd_list(t_cmd **cmds);
int		ft_copy_env(char **arr, t_env *env);
char	**ft_env_to_array(t_env *env);

/* exec/exec.c */
void	ft_run_child(t_cmd *cmd, char *exec_path, char **envp);
void	ft_set_child_status(t_data *data, int status);

/* exec/exec2.c */
void	ft_print_exec_error(char *cmd);
int		ft_prepare_exec(t_data *data, t_cmd *cmd,
			char **exec_path, char ***envp);
void	ft_exec_external_child(t_cmd *cmd, char *exec_path, char **envp);
int		ft_run_external_cmd(t_data *data, t_cmd *cmd);
int		ft_exec_simple_cmd(t_data *data, t_cmd *cmd);

/* exec/exec3.c */
int		ft_is_directory_path(const char *path);
int		ft_backup_std_fds(int *in_fd, int *out_fd);
void	ft_restore_std_fds(int in_fd, int out_fd);
int		ft_run_builtin_cmd(t_data *data, t_cmd *cmd);

/* pipe/pipe_utils.c */
int		ft_build_pipe_cmds(t_token *tokens, t_cmd **head);
int		ft_add_pipe_segment(t_token *start, t_token *pipe_pos, t_cmd **head);
int		ft_append_last_pipe_cmd(t_token *start, t_cmd **head);

/* pipe/pipe_utils2.c */
int		ft_count_cmds(t_cmd *cmd);
void	ft_setup_child_io(t_cmd *cur, int prev_fd, int pipefd[2]);
int		ft_redir_child_cmd(t_cmd *cur);
void	ft_update_parent_pipe(int *prev_fd, t_cmd *cur, int pipefd[2]);

/* pipe/pipe.c */
void	ft_run_child_cmd(t_data *data, t_cmd *cur, int prev_fd, int pipefd[2]);
int		ft_fork_pipe_cmds(t_data *data, t_cmd *cmd, pid_t *pids);
void	ft_wait_pipel_child(t_data *data, pid_t *pids, int count);
int		ft_exec_pipe(t_data *data, t_cmd *cmd);

/* pipe/pipe2.c */
void	ft_exec_path_or_exit(char *path, t_cmd *cur, char **envp);
void	ft_run_external_child_cmd(t_data *data, t_cmd *cur);

/* builtin/builtin.c */
int		ft_is_builtin(char *cmd);
int		ft_execute_builtin(t_data *data, t_cmd *cmd);

/* builtin/builtin_echo.c */
int		ft_builtin_echo(t_data *data, t_cmd *cmd);

/* builtin/builtin_cd.c */
void	ft_cd_error(char *arg);
void	ft_cd_set_pwd(t_data *data, char *oldpwd, char *cwd);
int		ft_cd_get_path(t_data *data, t_cmd *cmd, char **path);
int		ft_cd_change_dir(char *path, char **oldpwd);
int		ft_builtin_cd(t_data *data, t_cmd *cmd);

/* builtin/builtin_pwd.c */
int		ft_builtin_pwd(t_data *data, t_cmd *cmd);

/* builtin/builtin_export.c */
int		ft_export_set_name(t_data *data, char *arg);
int		ft_export_set_assignment(t_data *data, char *arg, char *equal);
int		ft_export_set_value(t_data *data, char *arg);
int		ft_builtin_export(t_data *data, t_cmd *cmd);

/* builtin/builtin_export_utils.c */
int		ft_export_valid_name(char *name);
int		ft_export_invalid_name(char *name, char *equal);
int		ft_export_find_name(t_env *export, char *name);
void	ft_export_print_entry(char *entry);

/* builtin/builtin_export_print.c */
int		ft_array_count(char **array);
char	**ft_merge_export_lists(char **env_list, char **exp_list);
void	ft_print_sorted_export(t_data *data);

/* builtin/builtin_unset.c */
int		ft_builtin_unset(t_data *data, t_cmd *cmd);

/* builtin/builtin_env.c */
int		ft_builtin_env(t_data *data, t_cmd *cmd);

/* builtin/builtin_exit.c */
int		ft_isnum_exit_arg(char *arg);
// int		ft_exit_many_arg(t_data *data);
int		ft_exit_num_required(t_data *data, char *arg);
int		ft_builtin_exit(t_data *data, t_cmd *cmd);

/* heredoc/heredoc_utils.c */
char	*ft_get_heredoc_delimiter(char *str);
int		ft_write_heredoc_to_pipe(char *delim, int fd);
int		ft_append_input_to_fd(char *infile, int fd);

/* heredoc/heredoc.c */
void	ft_run_heredoc_child(t_cmd *cmd, int pipefd[2]);
int		ft_wait_heredoc_child(pid_t pid, int pipefd[2]);
int		ft_start_heredoc_child(t_cmd *cmd, int pipefd[2]);
int		ft_prepare_heredoc_cmd(t_cmd *cmd);
int		ft_prepare_heredoc_list(t_cmd *cmds);

/* expand/expand_utils3.c */
int		ft_handle_double_dollar(char **result, size_t *i);

#endif