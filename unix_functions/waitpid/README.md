## waitpid()

`waitpid()` makes a parent process wait for a specific child process to finish.

It is declared in:

```c
#include <sys/wait.h>
```

### Function prototype

```c
pid_t waitpid(pid_t pid, int *status, int options);
```

Parameters:

- `pid`: which child process to wait for
- `status`: stores how the child ended
- `options`: usually `0`, meaning wait until the child finishes

### Basic example

```c
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}

	if (pid == 0)
	{
		printf("Child process\n");
		return (42);
	}

	printf("Parent is waiting...\n");
	waitpid(pid, &status, 0);

	if (WIFEXITED(status))
		printf("Child exited with status: %d\n", WEXITSTATUS(status));

	return (0);
}
```

Possible output:

```text
Parent is waiting...
Child process
Child exited with status: 42
```

### How `status` works

You should inspect `status` with macros:

```c
if (WIFEXITED(status))
{
	printf("Normal exit: %d\n", WEXITSTATUS(status));
}
```

Useful macros:

```c
WIFEXITED(status)
```

Checks whether the child finished normally.

```c
WEXITSTATUS(status)
```

Gets the child's exit code.

```c
WIFSIGNALED(status)
```

Checks whether the child was terminated by a signal.

```c
WTERMSIG(status)
```

Gets the signal that terminated the child.

## In your minishell

Your code contains:

```c
waitpid(pid, NULL, 0);
```

This means:

- Wait for the child with process ID `pid`
- Do not save its exit status because `NULL` is provided
- Wait until the child finishes because the option is `0`

For example:

```c
pid = fork();

if (pid == 0)
{
	exec(cmd);
	exit(1);
}
else
{
	waitpid(pid, NULL, 0);
}
```

The parent shell waits for the command to finish before displaying the next prompt.

Without `waitpid()`, the shell could display the next prompt immediately while the command is still running.