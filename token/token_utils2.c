/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 20:15:50 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 17:49:51 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
