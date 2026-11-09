/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 02:18:49 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 17:40:34 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: save the input filename into `cmd->infile`.
 * 
 * Saves the input filename into `cmd->infile`
 * Frees the old value first so there is no memory leak
 * Used when the token is `<`
 * 
 * How it works:
 * 1. frees old value
 * 2. stores new file path

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
 * Purpose: save output target and append mode.
 * 
 * Saves the output filename into cmd->outfile
 * Stores whether it is append mode in cmd->append
 * `append == 0` means `>`
 * `append == 1` means `>>`
 * 
 * How it works:
 * 1. frees old outfile
 * 2. stores `cmd->outfile`
 * 3. sets `cmd->append` to 0 or 1
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
 * Purpose: store heredoc delimiter text.
 * 
 * How it works:
 * 1. frees old heredoc value
 * 2. assigns new delimiter
 */
void	set_heredoc(t_cmd *cmd, char *value)
{
	if (cmd->heredoc)
		free(cmd->heredoc);
	cmd->heredoc = value;
}

/**
 * Purpose: dispatch redirection type to proper field.
 * 
 * How it works:
 * 1. `T_REDIR_IN` -> input file
 * 2. `T_REDIR_OUT` -> output file with truncate
 * 3. `T_APPEND` -> output file with append
 * 4. `T_HEREDOC` -> heredoc delimiter
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
