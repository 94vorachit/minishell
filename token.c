/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 16:10:37 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/27 19:48:34 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_free_matrix(char **matrix)
{
	int i;

	i = 0;
	while (matrix && matrix[i])
	{
		free(matrix[i]);
		matrix[i++] = NULL;
	}
	if (matrix)
		free(matrix);
	matrix = NULL;
}

static int	add_len(char *token)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (token[i])
	{
		if (token[i] == '|')
			count += 2;
		else if (token[i] == '>')
		{
			if (token[i + 1] && token[i + 1] == '>')
				i += 2;
			count += 2;
		}
		else if (token[i] == '<')
		{
			if (token[i + 1] && token[i + 1] == '<')
				i += 2;
			count += 2;
		}
		i++;
	}
	return (count + 1);
}

int	ft_token_len(char *line)
{
	char	**tokens;
	int		i;
	int		len;

	i = 0;
	len = 0;
	tokens = ft_split(line, ' ');
	while (tokens[i])
	{
		len += add_len(tokens[i]);
		i++;
	}
	ft_free_matrix(tokens);
	return (len);
}

/**
 * The function allocates memory for token_array and quote_array fields
 * of token struct and initializes the array size to 0.
 **/

static void	ft_init_token(char *line)
{
	int	num;

	num = ft_token_len(line) + 1;
	g_data->tokens->size = 0;
	g_data->tokens->token = (char **)malloc(sizeof(char *) * num);
	g_data->tokens->cmd = (char **)malloc(sizeof(char *) * num);
}

/**
 * The function adds the token and quote to the token_array and
 * quote_array of the tokens field in g_mini.
 **/

static void	ft_add_token_value(char *token, char* quote)
{
	int	index;

	index = g_data->tokens->size;
	g_data->tokens->token[index] = token;
	g_data->tokens->cmd[index] = quote;
	g_data->tokens->size++;
	// g_mini.tokens->token_array[index] = token;
	// g_mini.tokens->quote_array[index] = quote;
	// g_mini.tokens->size++;
}

/**
 * The function takes a line as an argument and tokenizes it.
 * It also deals with expansions and memory allocations.
 **/

void	ft_get_tokens(char *line)
{
	int			i;
	char		*token;									// temp var to store a token
	char		*quote;									// temp var to store a quoted token

	i = 0;
	g_data->tokens = ft_calloc(1, sizeof(t_token));
	if (line)
		ft_init_token(line);
	else
		return ;
	/* Iterate over line and extracts tokens */
	while (line[i])
	{
		/* extracts a token and returns index of next word */
		i += extract_token(&line[i], &token, &quote);
		/* if we got token and quote add them to appropriate arrays */
		if (token != NULL && quote != NULL)
			ft_add_token_value(token, quote);
	}
	/* Terminate arrays with NULLs */
	ft_add_token_value(NULL, NULL);
}
