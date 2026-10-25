### getcwd()

`getcwd` means “get current working directory.”
In C, `getcwd()` returns the absolute path of the directory where the current process is running.

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("Current directory: %s\n", cwd);

    return 0;
}
```

If you run the program from `/home/user/project`, it prints:
```c
Current directory: /home/user/project
```

#### Function signature:
```c
char *getcwd(char *buffer, size_t size);
```
* buffer: memory where the path is stored
* size: size of that buffer
* Returns buffer on success
* Returns NULL on failure

In a shell such as minishell, the pwd command commonly uses getcwd() to display the current directory.


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

### tcgetattr()

`tcgetattr` is a POSIX function that **gets the current terminal settings**.

It is declared in:

```c
#include <termios.h>
```

Its prototype is:

```c
int tcgetattr(int fd, struct termios *termios_p);
```

### In your code

```c
tcgetattr(0, &g_data->old);
tcgetattr(0, &g_data->new);
```

The `0` is the file descriptor for **standard input**, which is normally the terminal.

The function reads the terminal’s current configuration and stores it in a `struct termios`:

```c
struct termios old;
struct termios new;
```

Your code stores the same current terminal configuration twice:

- `g_data->old`: backup of the original settings.
- `g_data->new`: copy that can be modified.

The return value is:

- `0` on success.
- `-1` on failure, with `errno` indicating the reason.

### Why it is used here

The shell modifies the copied settings:

```c
g_data->new.c_lflag &= ~(ECHOCTL);
```

Then applies them:

```c
tcsetattr(0, TCSANOW, &g_data->new);
```

So the sequence is:

```text
tcgetattr  -> read current terminal settings
modify     -> disable ECHOCTL
tcsetattr  -> apply the modified settings
```

`ECHOCTL` controls whether terminal control characters, such as `Ctrl-C` and `Ctrl-\`, are visibly echoed as `^C` and `^\`.

In short, `tcgetattr` is used to **inspect and save the terminal configuration before changing it**.