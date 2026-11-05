/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 02:18:49 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/05 18:16:49 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Saves the input filename into `cmd->infile`
 * Frees the old value first so there is no memory leak
 * Used when the token is `<`
 * 
 * Example:
 * input: `cat < file.txt`
 * result: `cmd->infile = "file.txt"`
 */
void	set_input_file(t_cmd *cmd, char *value)
{
	free(cmd->infile);
	cmd->infile = value;
}

/**
 * Saves the output filename into cmd->outfile
 * Stores whether it is append mode in cmd->append
 * `append == 0` means `>`
 * `append == 1` means `>>`
 * 
 * Example:
 * `echo hi > out.txt` → `cmd->outfile = "out.txt", cmd->append = 0`
 * `echo hi >> out.txt` → `cmd->outfile = "out.txt", cmd->append = 1`
 */
void	set_output_file(t_cmd *cmd, char *value, int append)
{
	free(cmd->outfile);
	cmd->outfile = value;
	cmd->append = append;
}

/**
 * Parser stores delimiter text like EOF
 */
void	set_heredoc(t_cmd *cmd, char *value)
{
	if (cmd->heredoc)
		free(cmd->heredoc);
	cmd->heredoc = value;
}

/**
 * Decides which field of t_cmd should receive the redirection
 * 
 * Handles:
 * `T_REDIR_IN` → input file
 * `T_REDIR_OUT` → output file with truncate
 * `T_APPEND` → output file with append
 * `T_HEREDOC` → heredoc delimit
 * 
 * If some unexpected type appears, it frees the duplicated string safely
 * This function is a dispatcher.
 */
void	assign_redirection(t_cmd *cmd, t_token_type type, char *value)
{
	if (type == T_REDIR_IN)
		set_input_file(cmd, value);
	else if (type == T_REDIR_OUT)
		set_output_file(cmd, value, 0);
	else if (type == T_APPEND)
		set_output_file(cmd, value, 1);
	else if (type == T_HEREDOC)
		set_heredoc(cmd, value);
	else
		free(value);
}

/**
 * This is the logic that converts a token like:
 * * < file
 * * > file
 * * >> file
 * into the shell command structure.
 * 
 * What it does:
 * 1. checks that the redirection has a filename after it
 * 2. rejects invalid syntax like >, >>, or < without a following word
 * 3. ignores heredoc here (T_HEREDOC) by returning 0
 * 4. duplicates the filename
 * 5. stores it in the command structure with assign_redirection
 * 
 * If something fails:
 * * returns -1
 */
int	set_redirection(t_cmd *cmd, t_token *token)
{
	char	*value;
	int	fd;

	if (!token->next || token->next->type != T_WORD)
		return (-1);
	value = ft_strdup(token->next->value);
	if (!value)
		return (-1);
	if (token->type == T_REDIR_OUT || token->type == T_APPEND)
	{
		if (token->type == T_APPEND)
			fd = open(value, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			if (errno == ENOENT)
			{
				ft_putstr_fd("minishell: no such file or directory: ", STDERR);
				ft_putstr_fd(value, STDERR);
				ft_putstr_fd("\n", STDERR);
			}
			else
				perror(value);
			free(value);
			return (ERROR);
		}
		close(fd);
	}
	assign_redirection(cmd, token->type, value);
	return (0);
}

/**
 * This is the main parser for redirections in a single command.
 * 
 * How it works:
 * * walks through the token list
 * * when it sees a redirection token (<, >, >>, ...), it calls set_redirection
 * * if parsing fails, it returns -1
 * * if redirection is found, it skips the filename token because the filename 
 * is not part of the command arguments
 */
int	parse_redirections(t_token *tokens, t_cmd *cmd)
{
	t_token	*cur;
	char	*value;
	int	ret;

	cur = tokens;
	while (cur && cur->type != T_PIPE)
	{
		if (ft_isredirection(cur->type))
		{
			if (!cur->next || cur->next->type != T_WORD)
				return (-1);
			if (cur->type == T_REDIR_IN)
			{
				value = ft_strdup(cur->next->value);
				if (!value)
					return (-1);
				if (access(value, F_OK) != 0)
				{
					free(cmd->infile);
					cmd->infile = value;
					return (SUCCESS);
				}
				free(value);
			}
			ret = set_redirection(cmd, cur);
			if (ret != 0)
				return (ret);
			cur = cur->next;
		}
		cur = cur->next;
	}
	return (SUCCESS);
}
