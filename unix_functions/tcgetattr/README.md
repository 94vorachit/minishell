### tcgetattr() & tcsetattr()

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


### `tcgetattr()` reads the current terminal settings. Usually, you:

1. Save the original terminal settings.
2. Read the settings with `tcgetattr()`.
3. Modify them.
4. Apply them with `tcsetattr()`.
5. Restore the original settings before exiting.

### Example: disable terminal echo

```c
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main(void)
{
    struct termios original;
    struct termios modified;
    char password[100];

    /* Read and save the current terminal settings */
    if (tcgetattr(STDIN_FILENO, &original) == -1)
    {
        perror("tcgetattr");
        return (1);
    }

    modified = original;

    /* Disable echoing typed characters */
    modified.c_lflag &= ~(ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &modified) == -1)
    {
        perror("tcsetattr");
        return (1);
    }

    printf("Enter password: ");
    fflush(stdout);

    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        perror("fgets");
    }

    /* Always restore the original terminal settings */
    if (tcsetattr(STDIN_FILENO, TCSANOW, &original) == -1)
    {
        perror("tcsetattr");
        return (1);
    }

    printf("\nPassword received.\n");
    return (0);
}
```

Compile:

```bash
cc terminal.c -o terminal
```

Run:

```bash
./terminal
```

### Useful flags

```c
modified.c_lflag &= ~(ECHO);      /* Hide typed characters */
modified.c_lflag &= ~(ICANON);    /* Read input character by character */
modified.c_lflag &= ~(ECHOCTL);   /* Do not display ^C or ^\ */
modified.c_lflag |= (ECHO);       /* Enable echo */
modified.c_lflag |= (ICANON);     /* Enable line-by-line input */
```

For a `minishell`, you commonly save the settings before starting `readline()` and restore them when the shell exits:

```c
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main(void)
{
    struct termios terminal_settings;
    char *line;

    if (tcgetattr(STDIN_FILENO, &terminal_settings) == -1)
    {
        perror("tcgetattr");
        return (1);
    }

    line = readline("minishell$ ");
    if (line == NULL)
        printf("exit\n");
    else
    {
        printf("You entered: %s\n", line);
        free(line);
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_settings) == -1)
    {
        perror("tcsetattr");
        return (1);
    }

    return (0);
}
```

`STDIN_FILENO` is file descriptor `0`, representing standard input.