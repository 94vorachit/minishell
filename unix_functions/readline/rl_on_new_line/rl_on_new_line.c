/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rl_on_new_line.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 03:06:29 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 03:08:01 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include <unistd.h>
#include <stdlib.h>

int	main(void)
{
    char	*line;

    line = readline("minishell> ");
    if (line == NULL)
        return (1);

    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line(); // does not print a newline. It only updates Readline's internal cursor state.

    free(line);
    return (0);
}
