I’ll inspect the provided C file and the repository context, then turn the implementation into a clear step-by-step build plan.

Read , lines 1 to 180
Completed with input: {
  "path": "/home/acer/.vscode-server/data/agentSessionData/c6d73e48-ecda-45cd-8edd-9bfacfc07a7a/attachments/e7a692b2-1ba7-4822-859b-6ea3f1206a76/main.c",
  "view_range": [
    1,
    180
  ]
}

Find files matching `**/*`

"Read" failed
Completed with input: {
  "path": "/home/acer/42_code/myGit/myGit/minishell/README.md"
}
Tool execution failed: Path does not exist

The file is the entry point of a 42-style `minishell`: it initializes shell state, reads a command line, expands and parses it, then executes tokens in a loop. I’m checking the project’s build and module layout so the steps match this codebase rather than being generic.

Read , lines 1 to 240
Completed with input: {
  "path": "/home/acer/42_code/myGit/minishell/ref/minishell/README.md",
  "view_range": [
    1,
    240
  ]
}

Read , lines 1 to 220
Completed with input: {
  "path": "/home/acer/42_code/myGit/minishell/ref/minishell/Makefile",
  "view_range": [
    1,
    220
  ]
}

Read , lines 1 to 260
Completed with input: {
  "path": "/home/acer/42_code/myGit/minishell/ref/minishell/includes/minishell.h",
  "view_range": [
    1,
    260
  ]
}

Read , lines 1 to 240
Completed with input: {
  "path": "/home/acer/42_code/myGit/minishell/ref/minishell/src/utils/parsing.c",
  "view_range": [
    1,
    240
  ]
}

Search for `^void\s+execute|^int\s+exec|^int\s+pipe|^char\s+\*prompt|^char\s+\*ft_pgross_...`

Search for `^## |^### `

Read , lines 1 to 220
Completed with input: {
  "path": "/home/acer/42_code/myGit/minishell/ref/minishell/src/exec/exec.c",
  "view_range": [
    1,
    220
  ]
}

Read , lines 1 to 180
Completed with input: {
  "path": "/home/acer/42_code/myGit/minishell/ref/minishell/src/signal/signal.c",
  "view_range": [
    1,
    180
  ]
}

This program is a small Unix shell, similar to Bash. Build it in the following order.

## 1. Define the project scope

Support these features first:

- Interactive prompt
- Command history
- Environment variables
- Built-in commands:
  - `echo`
  - `cd`
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`
- External commands such as `ls`, `cat`, and `grep`
- Pipes: `|`
- Redirections:
  - `>`
  - `>>`
  - `<`
  - `<<`
- Single and double quotes
- Signals:
  - `Ctrl-C`
  - `Ctrl-D`
  - `Ctrl-\`

The project already uses this structure:

- Entry point: `src/main.c`
- Header and structures: `includes/minishell.h`
- Built-ins: `src/builtins/`
- Parsing and expansion: `src/utils/`
- Execution: `src/exec/`
- Signals: `src/signal/`
- Linked-list structures: `src/lst/`

---

## 2. Create the project directories

```text
minishell/
├── Makefile
├── includes/
│   └── minishell.h
├── libft/
├── src/
│   ├── main.c
│   ├── builtins/
│   ├── exec/
│   ├── lst/
│   ├── signal/
│   └── utils/
└── tests/
```

Use `libft` for common string, memory, list, and conversion functions.

---

## 3. Create the global shell state

The program uses a global pointer:

```c
t_data *g_data;
```

The main state should contain:

```c
typedef struct s_data
{
    char            **env;
    int             lastret;
    int             hd_stop;
    struct termios  old;
    struct termios  new;
    t_hd            *hd;
    t_token         *token;
} t_data;
```

This state stores:

- The shell environment
- The previous command’s exit status
- Here-document state
- Terminal settings
- Parsed commands
- Signal-related information

The complete project version defines these structures in `minishell.h`.

---

## 4. Initialize the shell

When the program starts:

1. Check that no command-line arguments were passed.
2. Allocate `g_data`.
3. Copy the environment received from `main`.
4. Initialize the shell level using `SHLVL`.
5. Configure terminal attributes.
6. Register signal handlers.
7. Start the interactive loop.

This is done by `main()` and `initiate_values()` in `main.c`.

Basic flow:

```c
int main(int argc, char **argv, char **env)
{
    (void)argv;

    if (argc != 1)
        return (1);

    g_data = malloc(sizeof(t_data));
    if (!g_data)
        return (1);

    initiate_values(g_data, env);
    main_loop();
    return (g_data->lastret);
}
```

---

## 5. Configure terminal signals

The shell must behave differently from ordinary programs.

At the prompt:

- `Ctrl-C` displays a new prompt.
- `Ctrl-\` is ignored.
- `Ctrl-D` exits the shell.

Inside a child process:

- `Ctrl-C` should terminate the running command.
- `Ctrl-\` should display the quit message.

The handlers are implemented in `src/signal/signal.c`.

Example initialization:

```c
signal(SIGINT, sigint_handler);
signal(SIGQUIT, SIG_IGN);
```

Disable the terminal’s `Ctrl-O` display behavior with:

```c
tcgetattr(STDIN_FILENO, &g_data->old);
g_data->new = g_data->old;
g_data->new.c_lflag &= ~(ECHOCTL);
tcsetattr(STDIN_FILENO, TCSANOW, &g_data->new);
```

---

## 6. Display the prompt

Create a `prompt()` function that returns a string such as:

```text
user@machine:current_directory$
```

The existing implementation is in `src/utils/prompt.c`.

The prompt normally uses:

- `$USER`
- The current working directory
- `$PWD`
- A shell-specific prefix or suffix

---

## 7. Read input with Readline

Use GNU Readline:

```c
line = readline(prompt);
```

The program should:

1. Print the prompt.
2. Read one command line.
3. Add non-empty commands to history.
4. Detect `Ctrl-D`.
5. Free the prompt string.

The provided code does this in `main_3()`:

```c
pt = prompt();
*line = readline(pt);
free(pt);

if (!*line)
{
    ft_putstr_fd("\b\bexit\n", 2);
    ft_exit(0);
}

if (**line)
    add_history(*line);
```

This is the first stage of the loop in `main.c`.

---

## 8. Validate the raw command line

Before parsing, check for invalid syntax:

- Unclosed single quote
- Unclosed double quote
- A pipe at the beginning
- A pipe at the end
- Two invalid pipes together
- Redirection without a filename
- Invalid parentheses, if subshells are supported

For invalid syntax:

1. Print an error.
2. Set an appropriate exit status.
3. Free the input.
4. Return to the prompt.

Do not terminate the entire shell for a syntax error.

---

## 9. Handle quotes

Implement quote processing before tokenization.

### Single quotes

Inside single quotes, treat everything literally:

```text
echo '$USER'
```

Output:

```text
$USER
```

### Double quotes

Inside double quotes:

- Spaces are preserved.
- Environment variables are expanded.
- `$?` is expanded.
- Single quotes are ordinary characters.

Example:

```text
echo "$USER"
```

The quote-processing functions are located in files such as:

- `ft_pgross_str.c`
- `ft_chdollar.c`
- `purge_q.c`

---

## 10. Expand environment variables

Implement expansion for:

```text
$USER
$HOME
$PATH
$PWD
$OLDPWD
$?
```

The expansion algorithm should:

1. Scan the command character by character.
2. Detect `$`.
3. Read the variable name.
4. Search for it in `g_data->env`.
5. Replace `$NAME` with its value.
6. Replace `$?` with the previous exit status.
7. Do not expand variables inside single quotes.

The environment helper functions are declared in `minishell.h` and implemented in the utility files.

---

## 11. Normalize spaces

After quote processing and expansion:

- Remove unnecessary spaces.
- Preserve spaces inside quotes.
- Add spaces around operators when needed.
- Keep redirection operators together.

For example:

```text
echo hello>output
```

Should be interpreted like:

```text
echo hello > output
```

The main loop performs this processing:

```c
line = ft_epur_str(ft_chdir(ft_pgross_str(line)));
```

The relevant code is in `main.c`.

---

## 12. Tokenize the command

Convert the processed input into tokens.

For example:

```text
cat file.txt | grep hello > result.txt
```

Can become:

```text
[
    "cat file.txt",
    "|",
    "grep hello",
    ">",
    "result.txt"
]
```

The project represents parsed commands using `t_token`:

```c
typedef struct s_token
{
    char             *cmd;
    t_redirect        redirect;
    struct s_token    *sub_token;
    struct s_token    *prev;
    struct s_token    *next;
} t_token;
```

The tokenizer and parser are in:

- `parsing.c`
- `token.c`
- `token_2.c`

The parser entry point is:

```c
token = ft_parsing(line);
```

---

## 13. Parse pipes

Split the input into commands separated by `|`.

Example:

```text
ls -l | grep ".c" | wc -l
```

Becomes three commands:

```text
1. ls -l
2. grep ".c"
3. wc -l
```

Each command needs:

- Its argument list
- Its input file descriptor
- Its output file descriptor
- Its pipe connections
- Its redirections

Pipe execution is handled in:

- `pipe.c`
- `exec_2.c`

---

## 14. Parse redirections

Recognize these operators:

| Operator | Meaning |
|---|---|
| `>` | Create or truncate a file |
| `>>` | Create or append to a file |
| `<` | Read input from a file |
| `<<` | Read input until a delimiter |

Examples:

```bash
echo hello > output.txt
echo hello >> output.txt
cat < input.txt
cat << EOF
hello
EOF
```

Use `open()` with the appropriate flags:

```c
open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
open(file, O_RDONLY);
```

Redirection code is located in:

- `file_redirect.c`
- `file_redirect2.c`
- `file_redirect3.c`
- `file_redirect4.c`

---

## 15. Implement here-documents

For:

```bash
cat << EOF
hello
world
EOF
```

The shell should:

1. Read lines repeatedly.
2. Stop when a line equals the delimiter.
3. Store the input in a pipe or temporary file.
4. Connect that file descriptor to standard input.
5. Expand variables unless the delimiter is quoted.

The here-document implementation is in:

- `here_doc.c`
- `hd.c`
- `hd_2.c`

---

## 16. Implement built-in commands

Implement the built-ins before external command execution.

### `echo`

```bash
echo hello
echo -n hello
```

File:

- `echo.c`

### `cd`

```bash
cd /tmp
cd
cd -
cd ..
```

`cd` must update:

- `PWD`
- `OLDPWD`

File:

- `rep.c`

### `pwd`

Print the current directory using `getcwd()`.

### `export`

```bash
export NAME=value
export NAME
```

The environment variable must be validated and updated.

File:

- `export.c`

### `unset`

```bash
unset NAME
```

Remove variables from the shell environment.

File:

- `unset.c`

### `env`

Print environment variables that have values.

File:

- `env.c`

### `exit`

```bash
exit
exit 42
exit 1 2
exit abc
```

Handle:

- Numeric arguments
- Too many arguments
- Invalid numeric arguments
- Exit status modulo 256

File:

- `exit.c`

---

## 17. Decide whether a command runs in the parent or a child

This is important.

Run these built-ins in the parent when there is no pipe:

```text
cd
export
unset
exit
```

Otherwise their changes would disappear when the child process exits.

For example, this must change the shell directory:

```bash
cd /tmp
pwd
```

However, a built-in in a pipeline normally runs in a child process:

```bash
export A=1 | echo hello
```

The command dispatcher is in `exec.c`.

---

## 18. Execute external commands

For a non-built-in command:

1. Split the command into arguments.
2. Search for the executable in `$PATH`.
3. Use `fork()`.
4. In the child:
   - Apply redirections.
   - Connect pipes.
   - Call `execve()`.
5. In the parent:
   - Close unused file descriptors.
   - Call `waitpid()`.
   - Save the child’s exit status.

The basic external execution code is in `exec.c`.

Important system calls:

```c
fork();
execve();
waitpid();
access();
getcwd();
chdir();
```

---

## 19. Implement pipelines

For:

```bash
ls | grep .c | wc -l
```

The shell must:

1. Create a pipe for each connection.
2. Fork one child for each command.
3. Connect each child’s input and output using `dup2()`.
4. Close unused descriptors in both parent and children.
5. Wait for all children.

Example child setup:

```c
dup2(previous_pipe_read, STDIN_FILENO);
dup2(current_pipe[1], STDOUT_FILENO);
```

Always close descriptors that are no longer needed. Otherwise commands may hang while waiting for an EOF that never arrives.

---

## 20. Apply redirections before execution

For each command:

1. Save the original standard input/output if required.
2. Open the redirection file.
3. Call `dup2()`.
4. Close the original file descriptor.
5. Execute the command.
6. Restore standard input/output for the parent shell.

Example:

```c
fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);
close(fd);
```

The final redirection should take precedence:

```bash
echo hello > a.txt > b.txt
```

The output should go to `b.txt`.

---

## 21. Track exit statuses

After every command, update:

```c
g_data->lastret
```

Examples:

```bash
true
echo $?
```

Output:

```text
0
```

For an external process, inspect `waitpid()` status:

```c
if (WIFEXITED(status))
    g_data->lastret = WEXITSTATUS(status);
else if (WIFSIGNALED(status))
    g_data->lastret = 128 + WTERMSIG(status);
```

This value is used by `$?`.

---

## 22. Free all allocated memory

Every loop iteration allocates:

- The input line
- The prompt
- Expanded strings
- Token lists
- Argument arrays
- Redirection lists
- Here-document resources

Free them after execution:

```c
free(line);
ft_tokenclean_all(&token);
ft_free_tab(args);
```

Also close:

- Pipe file descriptors
- Redirection file descriptors
- Here-document descriptors

Memory cleanup helpers are in:

- `ft_free.c`
- `token_2.c`

---

## 23. Build the main loop

The complete command loop should conceptually be:

```c
while (1)
{
    line = readline(prompt());

    if (line == NULL)
        break;

    if (*line == '\0')
    {
        free(line);
        continue;
    }

    add_history(line);

    if (syntax_error(line))
    {
        free(line);
        continue;
    }

    line = expand_variables(line);
    line = process_quotes(line);
    line = normalize_spaces(line);

    token = parse(line);

    if (token_error(token))
    {
        free_tokens(token);
        free(line);
        continue;
    }

    execute(token);

    free_tokens(token);
    free(line);
}
```

The provided implementation divides this into:

- `main()`
- `initiate_values()`
- `main_3()`
- `main_2()`

See `main.c`.

---

## 24. Create the Makefile

The existing `Makefile` compiles:

- All `.c` files under `src`
- `libft.a`
- GNU Readline

Important compiler options:

```make
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lreadline
```

Build with:

```bash
make
```

Clean object files:

```bash
make clean
```

Remove all build output:

```bash
make fclean
```

Rebuild everything:

```bash
make re
```

Run the shell:

```bash
./minishell
```

---

## 25. Test in increasing difficulty

Start with simple commands:

```bash
echo hello
pwd
env
ls
```

Then test arguments:

```bash
echo one two three
ls -la
```

Test variables:

```bash
echo $USER
echo "$HOME"
echo '$HOME'
echo $?
```

Test built-ins:

```bash
cd /tmp
pwd
export TEST=hello
echo $TEST
unset TEST
```

Test redirections:

```bash
echo hello > file
cat < file
echo again >> file
cat file
```

Test pipes:

```bash
ls | wc -l
cat file | grep hello
cat file | grep hello | wc -l
```

Test here-documents:

```bash
cat << EOF
hello
$USER
EOF
```

Test signals:

- Press `Ctrl-C` at the prompt.
- Press `Ctrl-\` at the prompt.
- Press `Ctrl-D` at the prompt.
- Press `Ctrl-C` while a command is running.

Test invalid syntax:

```bash
|
echo hello |
echo >
echo "unclosed
cat << 
```

---

## Overall execution flow

The program’s complete flow is:

```text
Start program
    ↓
Validate arguments
    ↓
Allocate and initialize shell state
    ↓
Copy environment and update SHLVL
    ↓
Configure terminal and signals
    ↓
Display prompt
    ↓
Read command with readline
    ↓
Check Ctrl-D and empty input
    ↓
Validate syntax
    ↓
Process quotes
    ↓
Expand environment variables
    ↓
Normalize spaces and operators
    ↓
Tokenize command
    ↓
Parse pipes and redirections
    ↓
Execute built-in or external command
    ↓
Create pipes and child processes if necessary
    ↓
Wait for child processes
    ↓
Update exit status
    ↓
Free memory and file descriptors
    ↓
Return to the prompt
```