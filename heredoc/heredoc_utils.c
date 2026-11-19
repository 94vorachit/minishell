/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 19:47:53 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/19 20:29:29 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Purpose: extract heredoc delimiter text from a chunk containing `<<`.
 *
 * How it works: finds `<<`, skips spaces, then copies characters until
 * whitespace or another shell operator (`<`, `>`, `|`).
 */
char	*get_heredoc_delimiter(char *str)
{
	char	*pos;
	char	*end;

	pos = ft_strstr(str, "<<");
	if (!pos)
		return (NULL);
	pos += 2;
	while (*pos && ft_isspace((unsigned char)*pos))
		pos++;
	if (!*pos)
		return (NULL);
	end = pos;
	while (*end && !ft_isspace((unsigned char)*end)
		&& *end != '<' && *end != '>' && *end != '|')
		end++;
	return (ft_substr(pos, 0, end - pos));
}

/**
 * Purpose: read user input line by line until the delimiter is found.
 *
 * How it works:
 * 1. repeatedly calls `readline("heredoc> ")`
 * 2. if user presses Ctrl-D or EOF, it stops
 * 3. if the line equals the delimiter, it stops
 * 4. otherwise it writes the line to the pipe and adds a newline
 *
 * Example:
 * 1. delimiter: `END`
 * 2. user types:
 * * `hello`
 * * `world`
 * * `END`
 * 3. the function writes:
 * * `hello\nworld\n`
 *
 * This is the actual “capture heredoc content” step.
 */
int	write_heredoc_to_pipe(char *delim, int fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, delim) == 0)
		{
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	return (0);
}

/**
 * Purpose: append input-file contents after heredoc lines.
 *
 * How it works:
 * 1. opens the input file
 * 2. reads it in chunks
 * 3. writes each chunk to the heredoc pipe as-is
 */
int	append_input_file_to_fd(char *infile, int fd)
{
	int		file_fd;
	char	buffer[BUFFER_SIZE];
	ssize_t	read_bytes;

	file_fd = open(infile, O_RDONLY);
	if (file_fd < 0)
		return (-1);
	read_bytes = read(file_fd, buffer, sizeof(buffer));
	while (read_bytes > 0)
	{
		write(fd, buffer, read_bytes);
		read_bytes = read(file_fd, buffer, sizeof(buffer));
	}
	close(file_fd);
	return (0);
}
