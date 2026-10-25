## dup() and dup2()

`dup()` and `dup2()` duplicate file descriptors. They are used to redirect input and output.

Include:

```c
#include <unistd.h>
```

## File descriptors

Common file descriptors are:

```c
STDIN_FILENO   // 0: standard input
STDOUT_FILENO  // 1: standard output
STDERR_FILENO  // 2: standard error
```

## `dup()`

`dup()` duplicates a file descriptor and returns the lowest available file descriptor.

```c
int new_fd;

new_fd = dup(old_fd);
```

Example:

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
	int	file_fd;
	int	copy_fd;

	file_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (file_fd == -1)
	{
		perror("open");
		return (1);
	}

	copy_fd = dup(file_fd);
	if (copy_fd == -1)
	{
		perror("dup");
		close(file_fd);
		return (1);
	}

	write(copy_fd, "Hello\n", 6);

	close(file_fd);
	close(copy_fd);
	return (0);
}
```

Both `file_fd` and `copy_fd` refer to the same opened file.

## `dup2()`

`dup2()` duplicates one file descriptor into a specific descriptor.

```c
dup2(old_fd, new_fd);
```

Example:

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
	int	file_fd;

	file_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (file_fd == -1)
	{
		perror("open");
		return (1);
	}

	if (dup2(file_fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(file_fd);
		return (1);
	}

	close(file_fd);
	printf("This goes into output.txt\n");

	return (0);
}
```

Before `dup2()`:

```text
printf() -> terminal
```

After:

```c
dup2(file_fd, STDOUT_FILENO);
```

the output becomes:

```text
printf() -> output.txt
```

`dup2()` closes `STDOUT_FILENO` first if it is already open, then makes it refer to the same file as `file_fd`.

## Difference

```c
dup(old_fd);
```

Chooses the lowest available descriptor automatically.

```c
dup2(old_fd, new_fd);
```

Uses the exact descriptor you specify.

For example:

```c
int copy = dup(file_fd);
```

might return `3`, while:

```c
dup2(file_fd, 1);
```

specifically replaces standard output.

## Pipes in a minishell

For this command:

```bash
ls | grep .c
```

the first child sends its output to the pipe:

```c
close(pipefd[0]);
dup2(pipefd[1], STDOUT_FILENO);
close(pipefd[1]);

execve("/bin/ls", args, envp);
```

The second child reads input from the pipe:

```c
close(pipefd[1]);
dup2(pipefd[0], STDIN_FILENO);
close(pipefd[0]);

execve("/bin/grep", args, envp);
```

The result is:

```text
ls stdout -> pipe -> grep stdin
```

## Input redirection example

For:

```bash
cat < input.txt
```

a shell can implement the redirection like this:

```c
int	file_fd;

file_fd = open("input.txt", O_RDONLY);
if (file_fd == -1)
{
	perror("input.txt");
	return (1);
}

dup2(file_fd, STDIN_FILENO);
close(file_fd);

execve("/bin/cat", args, envp);
```

After `dup2()`, `cat` reads from `input.txt` instead of the terminal.

Always check errors from `dup()` and `dup2()`, and close descriptors that are no longer needed.