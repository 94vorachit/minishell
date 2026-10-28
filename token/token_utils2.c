/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 20:15:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 18:18:51 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/** Adds a new token to the token list.
 * 
 * 1. creates a new token with ft_token_new
 * 2. if creation fails, frees the value string and returns -1
 * 3. otherwise appends it to the list with ft_token_add_back
 * 
 * This is the standard way to insert a token into the linked list.
 */
int	ft_add_token(t_token **tokens, char *value, t_token_type type)
{
	t_token	*new_token;

	new_token = ft_token_new(value, type);
	if (!new_token)
	{
		free(value);
		return (-1);
	}
	ft_token_add_back(tokens, new_token);
	return (0);
}

/** Appends one character to a string.
 * 
 * 1. gets the current string length
 * 2. allocates a new string with one extra character
 * 3. copies the old content
 * 4. adds the new character
 * 5. frees the old string
 * 6. updates the pointer
 * 
 * This is used while building a word token character by character.
 */
int	ft_add_char(char **word, char c)
{
	char	*new_word;
	size_t	len;

	len = ft_strlen(*word);
	new_word = malloc(sizeof(char) * (len + 2));
	if (!new_word)
		return (-1);
	ft_memcpy(new_word, *word, len);
	new_word[len] = c;
	new_word[len + 1] = '\0';
	free(*word);
	*word = new_word;
	return (0);
}

/** Allocates and initializes a new token node.
 * 
 * It sets:
 * * value
 * * type
 * * next = NULL
 * 
 * This creates the linked-list node that stores one token.
 */
t_token	*ft_token_new(char *value, t_token_type type)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->value = value;
	new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

/** Appends a token to the end of the token list.
 * 
 * If the list is empty, it becomes the head.
 * Otherwise, it walks to the last node and links the new token there.
 * 
 * This keeps the list in input order.
 */
void	ft_token_add_back(t_token **tokens, t_token *new_token)
{
	t_token	*current;

	if (!*tokens)
	{
		*tokens = new_token;
		return ;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

/** Frees the entire token list.
 * 
 * 1. walks through each node
 * 2. frees current->value
 * 3. frees the token node itself
 * 4. sets the list pointer to NULL
 * 
 This is used when syntax errors occur or when the shell is cleaning up. 
 */
void	ft_token_clear(t_token **tokens)
{
	t_token	*current;
	t_token	*next;

	current = *tokens;
	while (current)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
	*tokens = NULL;
}
