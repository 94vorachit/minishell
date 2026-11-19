/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 19:47:53 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/19 15:15:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
		line = readline("heredoc> ");
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
 * Purpose: Append the content of the input file to the given file descriptor.
 *
 * How it works:
 * 1. opens the file with `O_RDONLY`
 * 2. reads it in chunks into `buffer`
 * 3. writes each chunk to the pipe
 * 4. adds a final newline if the file does not end with one
 * 5. closes the file and returns success or failure
 *
 * Note: This function is typically used to handle input redirection in a
 * heredoc scenario, where the content of a file needs to be appended to the
 * heredoc's pipe.
 */
int	append_input_file_to_fd(char *infile, int fd)
{
	int		file_fd;
	char	buffer[BUFFER_SIZE];
	char	last_char;
	ssize_t	read_bytes;

	file_fd = open(infile, O_RDONLY);
	if (file_fd < 0)
		return (-1);
	last_char = '\0';
	read_bytes = read(file_fd, buffer, sizeof(buffer));
	while (read_bytes > 0)
	{
		write(fd, buffer, read_bytes);
		last_char = buffer[read_bytes - 1];
		read_bytes = read(file_fd, buffer, sizeof(buffer));
	}
	if (last_char != '\0' && last_char != '\n')
		write(fd, "\n", 1);
	close(file_fd);
	return (0);
}
