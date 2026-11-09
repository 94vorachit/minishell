/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/11/09 22:10:05 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

volatile sig_atomic_t	g_signal = 0;

/**
 * This file sets up how the shell reacts when the user presses Ctrl+C or Ctrl+.
 * 
 * This is a helper function.
 * 
 * It disables the terminal’s control-character display flag.
 * In plain terms, it stops the shell from showing `^C` when `Ctrl+C` is pressed.
 *
 * How it works:
 * * `tcgetattr(0, &termios_p)` reads terminal settings
 * * `termios_p.c_lflag &= ~(ECHOCTL)`; clears the flag that
 * echoes control characters
 * * `tcsetattr(0, TCSANOW, &termios_p)` applies the new settings
 *
 * This helps make the prompt behave more cleanly in a minishell.
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
 * This is the handler for `SIGINT` (Ctrl+C).
 * 
 * 1. disables the control-character display
 * 2. prints a newline
 * 3. tells readline the cursor is on a new line
 * 4. clears the current input line
 * 5. redraws the prompt
 *
 * So when the user presses Ctrl+C, the shell prints a new prompt on the next
 * line instead of leaving the old input visible.
 * 
 * Important points:
 * `rl_on_new_line()`; resets readline state
 * `rl_replace_line("", 0)`; clears the current input buffer
 * `rl_redisplay()`; redraws the prompt
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
 * This is the setup function for the signal handlers.
 * 
 * It registers:
 * 1. `SIGINT` → `sig_reprompt`
 * 2. `SIGQUIT` → ignored
 * 
 * This means the shell is configured during startup to react to user 
 * interrupt signals properly.
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

