`sig_disabled_display()` disables the terminal’s display of control characters, especially `Ctrl-C`.

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

This makes the shell prompt behave more cleanly:

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

Creates a structure holding terminal settings.

```c
tcgetattr(STDIN_FILENO, &termios_p)
```

Reads the current settings of standard input.

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
tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
```

Applies the modified settings immediately.

## Why it is used in a minishell

A shell must:

- receive `SIGINT` when the user presses `Ctrl-C`
- avoid printing `^C` itself
- ignore `Ctrl-\` (`SIGQUIT`) at the prompt
- restore normal terminal behavior for child processes

A minimal setup looks like:

```c
void sig_init(void)
{
    struct sigaction int_act;
    struct sigaction quit_act;

    sigemptyset(&int_act.sa_mask);
    sigemptyset(&quit_act.sa_mask);

    int_act.sa_handler = sig_reprompt;
    int_act.sa_flags = 0;

    quit_act.sa_handler = SIG_IGN;
    quit_act.sa_flags = 0;

    sigaction(SIGINT, &int_act, NULL);
    sigaction(SIGQUIT, &quit_act, NULL);
    sig_disabled_display();
}
```

## Why `sigaction()` is preferred over `signal()`

`signal()` is the old, simpler API.
`sigaction()` is the modern, safer API.

### `signal()`

- less portable
- behavior differs across systems
- harder to reason about when signals interrupt system calls

### `sigaction()`

- explicit and portable
- lets you control the signal mask and flags
- is the standard choice for shell implementations

For a shell, this matters because `readline`, `waitpid`, and child processes all interact with signals in subtle ways.

A typical shell pattern is:

```c
struct sigaction sa;

sa.sa_handler = sig_reprompt;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
sigaction(SIGINT, &sa, NULL);

sa.sa_handler = SIG_IGN;
sigaction(SIGQUIT, &sa, NULL);
```

This is why `sigaction()` is preferred in a minishell.

## Normal shell behavior

At the prompt:
- `Ctrl-C` clears the current input and redraws the prompt
- status becomes `130`
- `Ctrl-\` is ignored silently

While a foreground command is running:
- `Ctrl-C` sends `SIGINT` to the process
- the process is interrupted
- shell status becomes `130`
- `Ctrl-\` sends `SIGQUIT`, usually terminating the process with status `131`

This is the behavior a minishell should emulate.