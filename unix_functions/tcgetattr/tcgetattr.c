/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcgetattr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 19:23:36 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/23 19:28:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main(void)
{
    struct termios original;
    struct termios modified;
    char password[100];

    /* Read and save the current terminal settings */
    if (tcgetattr(STDIN_FILENO, &original) == -1)
    {
        perror("tcgetattr");
        return (1);
    }

    modified = original;

    /* Disable echoing typed characters */
    // modified.c_lflag &= ~(ECHO);
    /* Enable echo */
    modified.c_lflag |= (ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &modified) == -1)
    {
        perror("tcsetattr");
        return (1);
    }

    printf("Enter password: ");
    fflush(stdout);

    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        perror("fgets");
    }

    /* Always restore the original terminal settings */
    if (tcsetattr(STDIN_FILENO, TCSANOW, &original) == -1)
    {
        perror("tcsetattr");
        return (1);
    }

    printf("\nPassword received.\n");
    return (0);
}
