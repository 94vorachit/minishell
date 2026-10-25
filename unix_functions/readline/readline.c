/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 19:10:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/23 19:21:59 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

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
        printf("You entered: %s\n", line);
        free(line);
    }
    return (0);
}

/*
Compile it with:
```
cc readline.c -o readline -lreadline
```
*/