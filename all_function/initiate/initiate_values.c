/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initiate_values.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 14:41:57 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/23 14:42:24 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void	initiate_values(t_data *g_data, char **env)
{
	g_data->lastret = 0;
	g_data->env = env;
	g_data->env = array_dup();
	g_data->hd = 0;
	shlvlhandler(NULL, NULL);
	tcgetattr(0, &g_data->old);
	tcgetattr(0, &g_data->new);
	g_data->new.c_lflag &= ~(ECHOCTL);
	tcsetattr(0, TCSANOW, &g_data->new);
	signal(SIGINT, c_handler);
	signal(SIGQUIT, SIG_IGN);
}

