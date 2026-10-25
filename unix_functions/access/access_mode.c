/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   access.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 21:04:05 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/25 21:08:39 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

int main(void)
{
    // 1. Check if a file exists
    if (access("file.txt", F_OK) == 0)
        printf("file.txt exists\n");
    else
        printf("file.txt does not exist\n");
    // F_OK only checks existence.
    
    // 2. Check if a file is readable
    if (access("file.txt", R_OK) == 0)
        printf("The file can be read\n");
    else
        printf("The file cannot be read\n");

    // 3. Check if a file is writable
    if (access("file.txt", W_OK) == 0)
        printf("The file can be modified\n");
    else
        printf("The file cannot be modified\n");
    
    // 4. Check if a file is executable
    if (access("my_program", X_OK) == 0)
        printf("The file is executable\n");
    else
        printf("The file is not executable\n");
    
    // 5. Check multiple permissions
    if (access("script.sh", F_OK | X_OK) == 0)
        printf("script.sh exists and is executable\n");
    else
        printf("script.sh does not exist or is not executable\n");
    return 0;
}