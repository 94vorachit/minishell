### add_history()

`add_history.c` is currently an **empty, untracked file**—it contains 0 bytes and does not implement anything.

The actual history behavior comes from GNU Readline’s built-in `add_history()` function:

```c
if (**line)
    add_history(*line);
```

This appears in `ref/minishell/src/main.c:75`. It saves each non-empty command so the user can later navigate through previous commands with the Up/Down arrow keys.

The function is provided by:

```c
#include <readline/history.h>
```

and linked with:

```make
-lreadline
```

So `add_history.c` appears to be an unused placeholder or mistakenly created file; it should not contain a custom implementation unless the project specifically intends to replace Readline’s function.

Here is a simple example of how `add_history()` works in a shell:

```c
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(void)
{
    char *line;

    while (1)
    {
        line = readline("minishell$ ");

        if (line == NULL)
        {
            printf("exit\n");
            break;
        }

        if (*line != '\0')
            add_history(line);

        printf("You entered: %s\n", line);
        free(line);
    }

    return (0);
}
```

Compile it with:

```bash
cc main.c -lreadline -o shell
```

Run it:

```text
$ ./shell
minishell$ ls
You entered: ls
minishell$ pwd
You entered: pwd
minishell$ echo hello
You entered: echo hello
```

Now press the **Up arrow**:

```text
minishell$ echo hello
```

Press **Up** again:

```text
minishell$ pwd
```

Press **Up** again:

```text
minishell$ ls
```

This line is what saves the command:

```c
add_history(line);
```

Usually, it is protected by a check:

```c
if (*line)
    add_history(line);
```

That prevents empty input from being added to the history.

In your project, this code:

```c
*line = readline(pt);

if (**line)
    add_history(*line);
```

means:

1. Display the shell prompt.
2. Read the user’s command.
3. If the command is not empty, save it in Readline history.
4. The user can retrieve it later using the Up/Down arrow keys.

For example:

```text
minishell$ echo first
minishell$ echo second
```

After pressing Up:

```text
minishell$ echo second
```

After pressing Up again:

```text
minishell$ echo first
```