`suppress_output()` disables the terminal’s display of control characters, especially `Ctrl-C`.

The relevant line is:

```c
termios_p.c_lflag &= ~ECHOCTL;
```

## What `ECHOCTL` does

When `ECHOCTL` is enabled, pressing a control key such as `Ctrl-C` makes the terminal print:

```text
^C
```

When `ECHOCTL` is disabled, the terminal still sends the signal, but it does not display `^C`.

So this function makes the shell behave more cleanly:

```text
minishell ▸
```

instead of:

```text
minishell ▸ ^C
```

## Function breakdown

```c
struct termios termios_p;
```

Creates a structure containing terminal settings.

```c
tcgetattr(0, &termios_p)
```

Reads the current settings of file descriptor `0`, which is standard input.

```c
termios_p.c_lflag &= ~ECHOCTL;
```

Clears the `ECHOCTL` flag while preserving all other terminal settings.

Equivalent meaning:

```c
termios_p.c_lflag = termios_p.c_lflag & ~ECHOCTL;
```

The `~` creates a mask with every bit set except `ECHOCTL`, and `&=` clears only that flag.

```c
tcsetattr(0, 0, &termios_p)
```

Applies the modified settings to standard input.

The second argument `0` means `TCSANOW` on Linux: apply the changes immediately. It would be clearer to write:

```c
tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
```

## Why it is used in a minishell

Usually this is called when initializing interactive signal behavior:

```c
void sig_init(void)
{
    suppress_output();
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);
}
```

The shell needs to:

- Receive `SIGINT` when the user presses `Ctrl-C`.
- Avoid printing `^C` itself.
- Ignore `Ctrl-\` (`SIGQUIT`) at the prompt.
- Let child processes restore normal terminal behavior.

One issue in the shown function: if `tcgetattr` fails, the code still modifies and applies `termios_p`, even though it may not contain valid settings. A safer version is:

```c
static void suppress_output(void)
{
    struct termios termios_p;

    if (tcgetattr(STDIN_FILENO, &termios_p) == -1)
    {
        perror("minishell: tcgetattr");
        return;
    }
    termios_p.c_lflag &= ~ECHOCTL;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &termios_p) == -1)
        perror("minishell: tcsetattr");
}
```