## execve()

`execve()` runs a program by replacing the current process with a new program.

It is declared in:

```c
#include <unistd.h>
```

### Function prototype

```c
int execve(const char *path, char *const argv[], char *const envp[]);
```

Parameters:

- `path`: path to the executable
- `argv`: arguments passed to the program
- `envp`: environment variables

### Basic example

```c
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
	char	*args[] = {"ls", "-l", NULL};
	char	*envp[] = {NULL};

	execve("/bin/ls", args, envp);

	/* This runs only if execve fails */
	perror("execve");
	return (1);
}
```

The program is replaced by `/bin/ls`, so successful `execve()` does not return to the original code.

### Using `fork()` and `execve()`

This is the usual minishell pattern:

```c
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	pid_t	pid;
	int		status;
	char	*args[] = {"ls", "-l", NULL};
	char	*envp[] = {NULL};

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}

	if (pid == 0)
	{
		/* Child process becomes ls */
		execve("/bin/ls", args, envp);
		perror("execve");
		exit(127);
	}

	/* Parent waits for ls */
	waitpid(pid, &status, 0);
	return (0);
}
```

The sequence is:

```text
minishell
    |
    | fork()
    |
    +-- child process
    |       |
    |       +-- execve("/bin/ls", ...)
    |              becomes ls
    |
    +-- parent waits with waitpid()
```

### `argv` example

For this command:

```bash
echo hello world
```

The argument array should be:

```c
char	*args[] = {
	"echo",
	"hello",
	"world",
	NULL
};
```

The first element, `args[0]`, is conventionally the command name.

### Failure handling

If the path does not exist:

```c
char	*args[] = {"program", NULL};
char	*envp[] = {NULL};

execve("/wrong/path/program", args, envp);
perror("execve");
```

`execve()` returns `-1` only when it fails. Common errors include:

- File does not exist
- File is not executable
- Permission denied
- Invalid executable format

In a minishell, the child normally calls `exit(127)` after `execve()` fails.