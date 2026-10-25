### readline()

`readline` is a function from the GNU Readline library that reads a complete line of user input from the terminal.

`readline(pt)` reads a line of text from the user in the terminal.
```c
char *line = readline("minishell$ ");
```
Here:
* `pt` is the prompt string displayed to the user.
* The function waits for input until Enter is pressed.
* It returns the entered line as a dynamically allocated `char *`.
* It supports features such as arrow-key editing and command history.
* You must free the returned string when finished:

```c
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *line = readline(pt);

    if (line != NULL)
    {
        printf("You entered: %s\n", line);
        free(line);
    }
    return 0;
}
```
If the user sends `EOF`, usually with `Ctrl+D`, readline returns `NULL`.

In `readline`:

- **Press `Ctrl-D`** on an empty line to send EOF and exit.
- **Press `Ctrl-C`** to cancel the current input and show a new prompt.
- **Press `Ctrl-\`** to send `SIGQUIT` (usually ignored by the shell prompt).

In your shell code, detect `Ctrl-D` when `readline()` returns `NULL`:

```c
char *line = readline("minishell$ ");

if (line == NULL)
{
    printf("exit\n");
    return (0);
}
```

`readline()` returns `NULL` when the user presses `Ctrl-D` or when input reaches EOF.