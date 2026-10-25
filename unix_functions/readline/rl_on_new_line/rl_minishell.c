/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rl_minishell.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 03:08:33 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 03:16:45 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void	handle_sigint(int signal)
{
	(void)signal;
	rl_replace_line("", 0); //Clears the current input line.
	write(STDOUT_FILENO, "\n", 1); //Moves the terminal cursor to a new line.
	rl_on_new_line(); //Tells Readline that the cursor is now on a new line.
	rl_redisplay(); //Displays the prompt again.
}

int	main(void)
{
	char			*line;
	struct sigaction	sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return (1);
	}
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		line = readline("minishell> ");
		if (line == NULL)
		{
			write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		if (*line != '\0')
			add_history(line);
		if (*line != '\0')
			printf("You entered: %s\n", line);
		free(line);
	}
	return (0);
}
