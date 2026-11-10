/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/10 14:45:18 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

volatile sig_atomic_t	g_signal = 0;

/**
 * Disables terminal echo of control characters.
 *
 * This prevents the shell from showing `^C` when the user presses Ctrl+C,
 * while still allowing the signal itself to reach the program.
 *
 * Implementation:
 * - `tcgetattr()` reads the current terminal settings
 * - `ECHOCTL` is cleared from `c_lflag`
 * - `tcsetattr()` applies the new settings immediately
 */
void	sig_disabled_display(void)
{
	struct termios	termios_p;

	if (tcgetattr(0, &termios_p))
		perror("minishell: tcgetattr");
	termios_p.c_lflag &= ~(ECHOCTL);
	if (tcsetattr(0, TCSANOW, &termios_p))
		perror("minishell: tcsetattr");
}

/**
 * Interactive SIGINT handler.
 *
 * When the user presses Ctrl+C at the prompt, the shell clears the current
 * readline buffer, prints a newline, and redraws the prompt.
 *
 * Behavior:
 * - set `g_signal = SIGINT`
 * - call `rl_on_new_line()`
 * - clear the current input line
 * - redisplay the prompt
 */
void	sig_reprompt(int sig)
{
	(void)sig;
	sig_disabled_display();
	g_signal = SIGINT;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * Installs the interactive shell signal handlers.
 *
 * - `SIGINT` is handled by `sig_reprompt`
 * - `SIGQUIT` is ignored while waiting for user input
 *
 * This is the setup used by the main prompt loop.
 */
void	sig_init(void)
{
	struct sigaction	int_act;
	struct sigaction	quit_act;

	sigemptyset(&int_act.sa_mask);
	sigemptyset(&quit_act.sa_mask);
	int_act.sa_handler = sig_reprompt;
	int_act.sa_flags = 0;
	quit_act.sa_handler = SIG_IGN;
	quit_act.sa_flags = 0;
	sigaction(SIGINT, &int_act, NULL);
	sigaction(SIGQUIT, &quit_act, NULL);
	sig_disabled_display();
}

/**
 * Notes:
 * - `SIGINT` must update `g_signal` so the prompt loop can set the shell
 *   status to `128 + SIGINT`.
 * - `SIGQUIT` is intentionally ignored while idle, matching normal shell
 *   behavior at the prompt.
 * - `sig_init()` may be called repeatedly, so it must not reset `g_signal`.
 */