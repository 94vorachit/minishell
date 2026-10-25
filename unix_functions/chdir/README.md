### chdir()

`chdir()` changes the current working directory of the calling process. In a minishell, it is used to implement the `cd` builtin.

Search Tools

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    if (chdir("/tmp") == 0)
        printf("Directory changed successfully\n");
    else
        perror("chdir");

    return 0;
}
```

`chdir()` returns:

- `0` if the directory change succeeds
- `-1` if it fails

For example:

```c
if (chdir("/home/user/Documents") == -1)
    perror("cd");
```

If `/home/user/Documents` does not exist or you do not have permission, `perror()` prints the reason.

In a minishell, the `cd` builtin could use:

```c
int builtin_cd(char *path)
{
    if (chdir(path) == -1)
    {
        perror("minishell: cd");
        return (1);
    }
    return (0);
}
```

Example:

```bash
cd /tmp
pwd
```

Before `cd`:

```text
/home/user/minishell
```

After `chdir("/tmp")`:

```text
/tmp
```

Important: `chdir()` changes the directory only for the current process. Therefore, `cd` must run inside the shell process, not in a child process. If it runs in a child, the child changes directory but the shell stays in the original directory.

### Example

Here is a complete example using `chdir()`:

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <directory>\n", argv[0]);
		return (1);
	}

	if (chdir(argv[1]) == -1)
	{
		perror("chdir");
		return (1);
	}

	printf("Directory changed successfully\n");

	return (0);
}
```

Compile and run:

```bash
gcc main.c -o change_dir
./change_dir /tmp
```

However, this does **not** change the directory of your terminal after the program exits. The directory changes only inside the program process.

For a minishell `cd` builtin:

```c
#include <unistd.h>
#include <stdio.h>

int	builtin_cd(char *path)
{
	if (path == NULL)
	{
		printf("minishell: cd: missing argument\n");
		return (1);
	}

	if (chdir(path) == -1)
	{
		perror("minishell: cd");
		return (1);
	}

	return (0);
}
```

Example usage:

```c
int	main(void)
{
	if (builtin_cd("/tmp") == 0)
		printf("Now inside /tmp\n");

	return (0);
}
```

For multiple arguments, a shell might call it like this:

```c
char	*args[] = {"cd", "/tmp", NULL};

builtin_cd(args[1]);
```

`chdir("/tmp")` changes the current directory of the minishell process to `/tmp`.