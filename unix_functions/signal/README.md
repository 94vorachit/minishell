## signal()

`signal()` is used to define what a program should do when it receives a **signal** from the operating system.

Signals are notifications such as:

- `SIGINT` — interrupt, usually from pressing `Ctrl+C`
- `SIGQUIT` — quit, usually from pressing `Ctrl+\`
- `SIGTERM` — request termination
- `SIGCHLD` — a child process has finished

### Basic example

```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void	handle_sigint(int signum)
{
	(void)signum;
	printf("\nYou pressed Ctrl+C\n");
}

int	main(void)
{
	signal(SIGINT, handle_sigint);

	while (1)
	{
		printf("Running...\n");
		sleep(1);
	}

	return (0);
}
```

When you press `Ctrl+C`, the function `handle_sigint()` is called instead of the program immediately terminating.

### Ignoring a signal

```c
signal(SIGINT, SIG_IGN);
```

`SIG_IGN` means **ignore this signal**.

### Restoring the default behavior

```c
signal(SIGINT, SIG_DFL);
```

`SIG_DFL` means use the operating system's default behavior.

### Minishell example

A shell usually handles `Ctrl+C` differently from a child command:

```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void	handle_sigint(int signum)
{
	(void)signum;
	write(STDOUT_FILENO, "\n", 1);
	printf("minishell> ");
}

int	main(void)
{
	signal(SIGINT, handle_sigint);

	while (1)
	{
		/* Read and execute shell commands */
	}

	return (0);
}
```

In a minishell:

- The shell may catch `SIGINT` so `Ctrl+C` displays a new prompt.
- A child process running a command should usually use the default behavior.
- `Ctrl+D` is not a signal; it represents end-of-file from the terminal.

For more reliable signal handling in larger programs, `sigaction()` is generally preferred over `signal()`, but `signal()` is simpler for learning and small examples.