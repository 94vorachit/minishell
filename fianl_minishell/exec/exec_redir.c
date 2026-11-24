/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 02:18:49 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/20 20:09:45 by vorhansa         ###   ########.fr       */
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
void	ft_set_input_file(t_cmd *cmd, char *value)
{
	free(cmd->infile);
	cmd->infile = value;
	cmd->input_heredoc = 0;
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
void	ft_set_output_file(t_cmd *cmd, char *value, int append)
{
	free(cmd->outfile);
	cmd->outfile = value;
	cmd->append = append;
}

/**
 * Purpose: store one heredoc delimiter for a command.
 *
 * How it works:
 * 1. counts existing heredoc delimiters
 * 2. grows the delimiter array
 * 3. appends the new delimiter
 * 4. keeps a copy in `cmd->heredoc` for the active heredoc
 */
void	ft_set_heredoc(t_cmd *cmd, char *value)
{
	char	**new_list;
	int		count;

	count = 0;
	while (cmd->heredocs && cmd->heredocs[count])
		count++;
	new_list = malloc(sizeof(char *) * (count + 2));
	if (!new_list)
	{
		free(value);
		return ;
	}
	count = 0;
	while (cmd->heredocs && cmd->heredocs[count])
	{
		new_list[count] = cmd->heredocs[count];
		count++;
	}
	new_list[count++] = value;
	new_list[count] = NULL;
	free(cmd->heredocs);
	cmd->heredocs = new_list;
	free(cmd->heredoc);
	cmd->heredoc = ft_strdup(value);
	cmd->input_heredoc = 1;
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
void	ft_assign_redir(t_cmd *cmd, t_token_type type, char *value)
{
	if (type == T_REDIR_IN)
		ft_set_input_file(cmd, value);
	else if (type == T_REDIR_OUT)
		ft_set_output_file(cmd, value, 0);
	else if (type == T_APPEND)
		ft_set_output_file(cmd, value, 1);
	else if (type == T_HEREDOC)
		ft_set_heredoc(cmd, value);
	else
		free(value);
}
