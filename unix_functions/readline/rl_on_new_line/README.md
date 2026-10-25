`rl_on_new_line()` is a GNU Readline function that tells Readline that the cursor has moved to a new line.

Include:

```c
#include <readline/readline.h>
```

### Basic example

```c
#include <readline/readline.h>
#include <unistd.h>

int	main(void)
{
	char	*line;

	line = readline("minishell> ");
	if (line == NULL)
		return (1);

	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();

	free(line);
	return (0);
}
```

However, `rl_on_new_line()` does **not** print a newline. It only updates Readline's internal cursor state.

### Common minishell signal-handler usage

When the user presses `Ctrl+C`, the shell often does this:

```c
#include <readline/readline.h>
#include <signal.h>
#include <unistd.h>

void	handle_sigint(int signal)
{
	(void)signal;
	rl_replace_line("", 0);
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}
```

The functions work together:

```c
rl_replace_line("", 0);
```

Clears the current input line.

```c
write(STDOUT_FILENO, "\n", 1);
```

Moves the terminal cursor to a new line.

```c
rl_on_new_line();
```

Tells Readline that the cursor is now on a new line.

```c
rl_redisplay();
```

Displays the prompt again.

Typical result after pressing `Ctrl+C`:

```text
minishell> some command^C
minishell>
```

Without `rl_on_new_line()`, Readline may redraw the prompt at the wrong cursor position.