## pipe()

`pipe()` creates a communication channel between processes. One process writes data into the pipe, and another process reads it.

It is declared in:

```c
#include <unistd.h>
```

### Function prototype

```c
int pipe(int pipefd[2]);
```

After calling `pipe()`:

```c
pipefd[0]  /* read end */
pipefd[1]  /* write end */
```

### Basic example

```c
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
	int	pipefd[2];
	char	buffer[100];
	int		bytes_read;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (1);
	}

	write(pipefd[1], "Hello from pipe\n", 16);

	bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
	buffer[bytes_read] = '\0';

	printf("Received: %s", buffer);

	close(pipefd[0]);
	close(pipefd[1]);

	return (0);
}
```

The data flow is:

```text
write(pipefd[1]) ---> pipe ---> read(pipefd[0])
```

### `pipe()` with `fork()`

A pipe is commonly used between a parent and child process:

```c
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	int		pipefd[2];
	pid_t	pid;
	char	buffer[100];
	int		bytes_read;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (1);
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}

	if (pid == 0)
	{
		/* Child writes into the pipe */
		close(pipefd[0]);
		write(pipefd[1], "Message from child\n", 19);
		close(pipefd[1]);
		exit(0);
	}
	else
	{
		/* Parent reads from the pipe */
		close(pipefd[1]);
		bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
		buffer[bytes_read] = '\0';
		printf("Parent received: %s", buffer);
		close(pipefd[0]);
		waitpid(pid, NULL, 0);
	}

	return (0);
}
```

### Pipes in a minishell

A shell command such as:

```bash
ls | grep .c
```

uses a pipe:

```text
ls stdout ---> pipe ---> grep stdin
```

The shell can connect these streams with `dup2()`:

```c
int	pipefd[2];
pid_t	pid1;
pid_t	pid2;

pipe(pipefd);

pid1 = fork();
if (pid1 == 0)
{
	/* First command: ls */
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	execve("/bin/ls", args1, envp);
	perror("execve");
	exit(127);
}

pid2 = fork();
if (pid2 == 0)
{
	/* Second command: grep */
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	execve("/bin/grep", args2, envp);
	perror("execve");
	exit(127);
}

/* Parent does not use the pipe */
close(pipefd[0]);
close(pipefd[1]);
waitpid(pid1, NULL, 0);
waitpid(pid2, NULL, 0);
```

Important points:

- `pipefd[0]` is for reading.
- `pipefd[1]` is for writing.
- `dup2(pipefd[1], STDOUT_FILENO)` sends the first command's output into the pipe.
- `dup2(pipefd[0], STDIN_FILENO)` makes the second command read from the pipe.
- Unused pipe ends must be closed, otherwise commands may wait forever for end-of-file.
