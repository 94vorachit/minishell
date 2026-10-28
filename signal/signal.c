/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 19:15:41 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 16:50:39 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * This file sets up how the shell reacts when the user presses Ctrl+C or Ctrl+.
 * This is a helper function.
 * It disables the terminal’s control-character display flag.
 * In plain terms, it stops the shell from showing ^C when Ctrl+C is pressed.
 *
 * How it works:
 * tcgetattr(0, &termios_p) reads terminal settings
 * termios_p.c_lflag &= ~(ECHOCTL); clears the flag that
 * echoes control characters
 * tcsetattr(0, TCSANOW, &termios_p) applies the new settings
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
 * This is the handler for SIGINT (Ctrl+C).
 * It:
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
 * rl_on_new_line(); resets readline state
 * rl_replace_line("", 0); clears the current input buffer
 * rl_redisplay(); redraws the prompt
 */
void	sig_reprompt(int sig)
{
	(void)sig;
	sig_disabled_display();
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * This is the handler for SIGQUIT (Ctrl+).
 * It does not exit the shell.
 * It only:
 * 1. disables control-character echo
 * 2. moves readline to a fresh line
 * 3. redisplays the prompt
 * 
 * This is usually used to ignore Ctrl+\ in an interactive shell.
 */
void	sig_quit(int sig)
{
	(void)sig;
	sig_disabled_display();
	rl_on_new_line();
	rl_redisplay();
}

/**
 * This is another signal handler, but it exits the shell.
 * It:
 * 1. disables control-character display
 * 2. refreshes readline state
 * 3. exits with status 1
 * 
 * This is used when a foreground program is interrupted and the shell should 
 * exit in a controlled way.
 */
void	sig_interrupt(int sig)
{
	(void)sig;
	sig_disabled_display();
	rl_on_new_line();
	rl_redisplay();
	exit(1);
}

/**
 * This is the setup function for the signal handlers.
 * It registers:
 * 1. SIGINT → sig_reprompt
 * 2. SIGQUIT → sig_quit
 * 
 * This means the shell is configured during startup to react to user 
 * interrupt signals properly.
 */
void	sig_init(void)
{
	signal(SIGINT, sig_reprompt);
	signal(SIGQUIT, sig_quit);
}
