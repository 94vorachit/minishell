## fork()

`fork()` creates a new process by copying the current process.

The original process is called the **parent**. The new process is called the **child**.

It is declared in:

```c
#include <unistd.h>
```

### Basic example

```c
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
	pid_t	pid;

	pid = fork();

	if (pid == -1)
	{
		perror("fork");
		return (1);
	}
	else if (pid == 0)
	{
		printf("This is the child process\n");
	}
	else
	{
		printf("This is the parent process\n");
	}

	return (0);
}
```

### Return values

`fork()` returns different values in each process:

```c
pid == -1
```

The creation failed.

```c
pid == 0
```

This code is running inside the child process.

```c
pid > 0
```

This code is running inside the parent process. The value is the child's process ID.

### Example with process IDs

```c
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
	pid_t	pid;

	pid = fork();

	if (pid == -1)
	{
		perror("fork");
		return (1);
	}

	if (pid == 0)
	{
		printf("Child PID: %d\n", getpid());
		printf("Parent PID: %d\n", getppid());
	}
	else
	{
		printf("Parent PID: %d\n", getpid());
		printf("Child PID: %d\n", pid);
	}

	return (0);
}
```

### `fork()` in a minishell

A minishell usually creates a child process before running an external command:

```c
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
	/* Child process */
	execve("/bin/ls", args, envp);
	perror("execve");
	exit(127);
}
else
{
	/* Parent minishell process */
	waitpid(pid, &status, 0);
}
```

The sequence is:

1. The shell calls `fork()`.
2. The child process is created.
3. The child calls `execve()` to become the command, such as `ls`.
4. The parent shell calls `waitpid()` and waits for the command to finish.
5. The shell displays the next prompt.

`fork()` is important because the shell must remain alive while the child runs the external command.