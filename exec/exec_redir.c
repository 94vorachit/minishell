/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 02:18:49 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/05 20:34:42 by vorhansa         ###   ########.fr       */
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
