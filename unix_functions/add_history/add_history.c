/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_history.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 17:10:33 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/23 17:13:30 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(void)
{
    char *line;

    while (1)
    {
        line = readline("minishell$ ");

        if (line == NULL)
        {
            printf("exit\n");
            break;
        }

        if (*line != '\0') // Usually, it is protected by a check:
            add_history(line); //This line is what saves the command:

        printf("You entered: %s\n", line);
        free(line);
    }

    return (0);
}

/*
cc main.c -lreadline -o shell
*/