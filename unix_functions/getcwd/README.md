### getcwd() [pwd]

`getcwd()` means **get current working directory**. It returns the absolute path of the directory where the current process is running.

### Basic example

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	char	*current_directory;

	current_directory = getcwd(NULL, 0);
	if (current_directory == NULL)
	{
		perror("getcwd");
		return (1);
	}

	printf("Current directory: %s\n", current_directory);
	free(current_directory);

	return (0);
}
```

Example output:

```text
Current directory: /home/user/minishell
```

### How it works

```c
getcwd(NULL, 0);
```

- `NULL` tells the system to allocate the buffer.
- `0` allows it to choose the required size.
- The returned string must be released with `free()`.

### Using `getcwd()` after `chdir()`

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	char	*directory;

	directory = getcwd(NULL, 0);
	printf("Before: %s\n", directory);
	free(directory);

	if (chdir("/tmp") == -1)
	{
		perror("chdir");
		return (1);
	}

	directory = getcwd(NULL, 0);
	if (directory == NULL)
	{
		perror("getcwd");
		return (1);
	}

	printf("After:  %s\n", directory);
	free(directory);

	return (0);
}
```

Possible output:

```text
Before: /home/user/minishell
After:  /tmp
```

### In a minishell

`getcwd()` is commonly used to implement the `pwd` builtin:

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	builtin_pwd(void)
{
	char	*directory;

	directory = getcwd(NULL, 0);
	if (directory == NULL)
	{
		perror("minishell: pwd");
		return (1);
	}

	printf("%s\n", directory);
	free(directory);
	return (0);
}
```

The shell can then execute:

```bash
pwd
```

and print the current working directory.