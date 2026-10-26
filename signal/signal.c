/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/26 20:13:09 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	sig_disabled_display(void)
{
	struct termios	termios_p;

	if (tcgetattr(0, &termios_p))
		perror("minishell: tcgetattr");
	termios_p.c_lflag &= ~(ECHOCTL);
	if (tcsetattr(0, TCSANOW, &termios_p))
		perror("minishell: tcsetattr");
}

static void	sig_reprompt(int sig)
{
	(void)sig;
	sig_disabled_display();
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	sig_quit(int sig)
{
	(void)sig;
	sig_disabled_display();
	rl_on_new_line();
	rl_redisplay();
}

void	sig_interrupt(int sig)
{
	(void)sig;
	sig_disabled_display();
	rl_on_new_line();
	rl_redisplay();
	exit(1);
}

void	sig_init(void)
{
	signal(SIGINT, sig_reprompt);
	signal(SIGQUIT, sig_quit);
}