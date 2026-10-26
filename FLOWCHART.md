# Minishell project flow

This project is a small Bash-like shell written in C. The main job is:

1. Show a prompt.
2. Read a command.
3. Parse the command.
4. Expand variables and wildcards.
5. Detect pipes and redirections.
6. Execute built-in or external commands.
7. Save the exit status.
8. Repeat until `exit` or `Ctrl-D`.

The main program is in `main.c`.

---

## 1. Complete flow chart

```text
                         ┌─────────────────────┐
                         │       main()        │
                         └──────────┬──────────┘
                                    │
                  Check argc == 1   │
                                    ▼
                         ┌─────────────────────┐
                         │ Allocate g_data     │
                         └──────────┬──────────┘
                                    │
                                    ▼
                         ┌─────────────────────┐
                         │ initiate_values()   │
                         │ - Copy environment  │
                         │ - SHLVL             │
                         │ - Signals           │
                         │ - Terminal settings │
                         └──────────┬──────────┘
                                    │
                                    ▼
                    ┌────────────────────────────┐
                    │         main_2()            │
                    │       Infinite loop         │
                    └─────────────┬──────────────┘
                                  │
                                  ▼
                         ┌─────────────────────┐
                         │    main_3()         │
                         │ - Display prompt    │
                         │ - readline()        │
                         │ - Add history       │
                         │ - Check Ctrl-D      │
                         └──────────┬──────────┘
                                    │
                         Empty/invalid command?
                              ┌─────┴─────┐
                              │           │
                            Yes           No
                              │           │
                              │           ▼
                              │   ┌─────────────────────┐
                              │   │ Preprocess command   │
                              │   │ - ft_pgross_str()   │
                              │   │ - ft_chdir()        │
                              │   │ - ft_epur_str()     │
                              │   └──────────┬──────────┘
                              │              │
                              │              ▼
                              │   ┌─────────────────────┐
                              │   │    ft_parsing()      │
                              │   │ - Split commands    │
                              │   │ - Detect operators  │
                              │   │ - Create tokens     │
                              │   │ - Detect heredocs   │
                              │   │ - Initialize files  │
                              │   └──────────┬──────────┘
                              │              │
                              │              ▼
                              │   ┌─────────────────────┐
                              │   │ ft_token_error()     │
                              │   └──────────┬──────────┘
                              │              │
                              │       Syntax error?
                              │         ┌────┴────┐
                              │         │         │
                              │       Yes         No
                              │         │         │
                              │         │         ▼
                              │         │  ┌─────────────────────┐
                              │         │  │    execute()         │
                              │         │  └──────────┬──────────┘
                              │         │             │
                              │         │             ▼
                              │         │  ┌─────────────────────┐
                              │         │  │ Expand command       │
                              │         │  │ - Variables         │
                              │         │  │ - Quotes            │
                              │         │  │ - Wildcards         │
                              │         │  └──────────┬──────────┘
                              │         │             │
                              │         │             ▼
                              │         │  ┌─────────────────────┐
                              │         │  │ Redirections        │
                              │         │  │ - <                 │
                              │         │  │ - >                 │
                              │         │  │ - >>                │
                              │         │  │ - <<                │
                              │         │  └──────────┬──────────┘
                              │         │             │
                              │         │             ▼
                              │         │  ┌─────────────────────┐
                              │         │  │ Pipe or normal cmd? │
                              │         │  └───────┬─────┬───────┘
                              │         │          │     │
                              │         │        Pipe   Normal
                              │         │          │     │
                              │         │          ▼     ▼
                              │         │  ┌──────────┐ ┌────────────┐
                              │         │  │exec_pipe │ │ redirect_  │
                              │         │  │          │ │ exec       │
                              │         │  └────┬─────┘ └──────┬─────┘
                              │         │       │              │
                              │         │       └──────┬───────┘
                              │         │              ▼
                              │         │     ┌─────────────────┐
                              │         │     │ Execute command │
                              │         │     └────────┬────────┘
                              │         │              │
                              │         │              ▼
                              │         │     ┌────────────────────┐
                              │         │     │ Built-in or system │
                              │         │     │ command?           │
                              │         │     └──────┬───────┬─────┘
                              │         │            │       │
                              │         │        Built-in  External
                              │         │            │       │
                              │         │            ▼       ▼
                              │         │    ┌────────────┐ ┌─────────┐
                              │         │    │ echo/cd/...│ │ fork()  │
                              │         │    └──────┬─────┘ │ execve()│
                              │         │           │       │ waitpid │
                              │         │           └───┬───┴─────────┘
                              │         │               ▼
                              │         │      Save exit status
                              │         │
                              │         └──────────────────────────┐
                              │                                    │
                              └────────────────────────────────────┘
                                                   │
                                                   ▼
                                         Repeat the shell loop
```

---

# 2. Important data structures

These are declared in `minishell.h`.

## `t_data`

Global shell state:

```c
typedef struct s_data
{
    char            **env;
    int             lastret;
    int             is_pipe;
    int             pipe[2];
    int             hd_stop;
    struct termios  old;
    struct termios  new;
} t_data;
```

It stores:

- Environment variables.
- Last command exit status.
- Pipe information.
- Heredoc state.
- Terminal configuration.
- Signal-related state.

The project uses:

```c
extern t_data *g_data;
```

This allows most files to access the shell state.

## `t_token`

Represents one parsed command:

```c
typedef struct s_token
{
    char            *cmd;
    t_redirect      redirect;
    struct s_token  *next;
    struct s_token  *prev;
    struct s_token  *sub_token;
} t_token;
```

For example:

```bash
cat file.txt | grep hello > result.txt
```

Can become approximately:

```text
Token 1: "cat file.txt"
Token 2: "|"
Token 3: "grep hello"
Token 4: ">"
Token 5: "result.txt"
```

## `t_redirect`

Stores input and output redirections:

```c
typedef struct s_redirect
{
    t_list *infile;
    t_list *outfile;
    int     infd;
    int     outfd;
} t_redirect;
```

---

# 3. Main functions

## `main()`

Located in `main.c`.

Responsibilities:

```text
Check arguments
        ↓
Allocate g_data
        ↓
Initialize the shell
        ↓
Start main_2()
```

The program accepts no command-line arguments:

```bash
./minishell
```

If the user runs:

```bash
./minishell something
```

the program prints an error.

---

## `initiate_values()`

Initializes the shell:

- Sets `lastret` to zero.
- Copies the original environment.
- Sets `SHLVL`.
- Saves terminal configuration.
- Disables terminal `Ctrl-\` echo behavior.
- Installs signal handlers.

Related files:

- `main.c`
- `signal.c`
- `signal2.c`

---

## `main_2()`

This is the main shell loop:

```c
while (1)
{
    read command
    preprocess command
    parse command
    validate tokens
    execute command
    free memory
}
```

It never stops normally. It stops when:

- The user types `exit`.
- The user presses `Ctrl-D`.
- A fatal error occurs.

---

## `main_3()`

Reads one command:

```text
prompt()
  ↓
readline()
  ↓
Add command to history
  ↓
Check empty input
  ↓
Check invalid syntax
```

Important behavior:

- `readline()` displays the command line.
- `add_history()` saves valid non-empty commands.
- `Ctrl-D` causes the shell to print `exit` and terminate.
- `Ctrl-C` is handled by a signal handler.

---

# 4. Step-by-step implementation order for a beginner

Do not implement the entire shell at once. Build it in small stages.

---

## Step 1: Learn the required C and Unix concepts

Before writing Minishell, understand:

### C

- Pointers.
- Arrays and strings.
- Structures.
- Linked lists.
- Dynamic memory.
- Function pointers.
- File descriptors.

### Unix functions

You will need:

```c
readline()✓
add_history()✓
fork()✓
execve()✓
waitpid()✓
pipe()✓
dup()✓
dup2()✓
open()✓
close()✓
access()✓
chdir()✓
getcwd()✓
signal()✓
tcgetattr()✓
tcsetattr()✓
```

### Recommended mini exercises

Write small programs for:

1. `fork()`.
2. `execve()`.
3. `pipe()`.
4. `dup2()`.
5. `waitpid()`.
6. Reading from a file.
7. Redirecting `stdout` to a file.
8. Handling `SIGINT`.

---

## Step 2: Build or understand `libft`

The `libft/` directory provides string and list functions.

Important functions include:

```text
ft_strlen()
ft_strdup()
ft_strjoin()
ft_split()
ft_substr()
ft_strcmp()
ft_strncmp()
ft_atoi()
ft_itoa()
ft_calloc()
ft_lstnew()
ft_lstadd_back()
ft_lstclear()
```

First make sure this works:

```bash
make -C libft
```

Then test the complete project:

```bash
make
```

---

## Step 3: Create the project structure

The current project is organized like this:

```text
minishell/
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
├── Makefile
└── README.md
```

The build rules are in `Makefile`.

---

## Step 4: Implement the basic prompt

Start with the smallest shell:

```c
while (1)
{
    line = readline("minishell$ ");
    if (line == NULL)
        break;
    if (*line != '\0')
        add_history(line);
    free(line);
}
```

At this stage, test:

```bash
make
./minishell
```

Expected behavior:

```text
minishell$ hello
minishell$ ls
minishell$ Ctrl-D
exit
```

Do not add parsing yet.

---

## Step 5: Add signal handling

Implement these behaviors:

| Key | Expected behavior |
|---|---|
| `Ctrl-C` | Cancel current input and show a new prompt |
| `Ctrl-D` | Exit the shell |
| `Ctrl-\` | Ignored while waiting for a command |
| `Ctrl-C` in child | Interrupt the running command |
| `Ctrl-\` in child | Quit the child process |

Main signal functions are in:

- `signal.c`
- `signal2.c`

---

## Step 6: Implement environment management

Copy `env` into your own environment array or linked list.

Required operations:

```text
Find an environment variable
Add or update a variable
Remove a variable
Convert a linked list to char **
Sort variables for export
```

Important functions include:

```text
my_getenv()
my_setenv()
findenv()
refresh_env()
list_env()
listtotab()
sort_exp()
```

Environment-related files:

- `env.c`
- `utils.c`
- `export.c`

Test:

```bash
env
export TEST=hello
env
unset TEST
env
echo $PATH
```

---

## Step 7: Implement simple command splitting

Start with commands separated only by spaces:

```bash
echo hello world
```

Convert the input into:

```text
["echo", "hello", "world", NULL]
```

At this stage, support:

- Command name.
- Arguments.
- Multiple spaces.
- Empty input.

Do not implement pipes or redirections yet.

---

## Step 8: Implement built-in commands

Built-ins must run inside the shell process because some of them modify shell state.

For example:

```bash
cd /tmp
```

must change the current directory of Minishell itself. If it runs only in a child process, the directory change disappears.

Implement them in this order:

### `pwd`

Print the current directory.

Function:

```text
pwd()
```

### `echo`

Print arguments.

Support:

```bash
echo hello
echo -n hello
```

Functions:

```text
echo()
echo_2()
```

### `env`

Print environment variables.

Function:

```text
print_env()
env_cmd()
```

### `export`

Add or update environment variables.

Examples:

```bash
export NAME=value
export
```

Functions:

```text
export()
checkvalid()
export_print()
```

### `unset`

Remove environment variables.

Example:

```bash
unset NAME
```

Functions:

```text
unset()
checkvalid_unset()
del_element()
```

### `cd`

Change the current directory and update:

```text
PWD
OLDPWD
```

Functions:

```text
cd()
cd_2()
goto_dir()
```

### `exit`

Exit the shell and return an exit status.

Examples:

```bash
exit
exit 0
exit 42
exit abc
```

Functions:

```text
exit_cmd()
exit_error()
exit_cmd_2()
```

Built-in source files are in `builtins/`.

---

## Step 9: Execute external commands

For a command such as:

```bash
ls -la
```

the shell should:

```text
Split command into arguments
        ↓
Find the executable using PATH
        ↓
fork()
        ↓
Child calls execve()
        ↓
Parent calls waitpid()
        ↓
Save exit status
```

Main execution functions:

```text
exec()
lexer()
cmd_sys()
not_pid()
get_path()
```

These are mainly in:

- `exec.c`
- `utils.c`

Example:

```text
Input:     ls -la
Arguments: ["ls", "-la", NULL]
Path:      /usr/bin/ls
Child:     execve("/usr/bin/ls", args, env)
Parent:    waitpid(child)
```

Test:

```bash
ls
ls -la
cat Makefile
/bin/echo hello
./minishell
unknown_command
```

---

## Step 10: Implement quotes

Quotes change how spaces and special characters behave.

### Single quotes

Everything is literal:

```bash
echo '$USER'
```

Output:

```text
$USER
```

### Double quotes

Variables are expanded:

```bash
echo "$USER"
```

Output:

```text
your_username
```

You need to track:

```text
inside_single_quote
inside_double_quote
```

Important functions include:

```text
ft_woquote()
ft_ecrase_q()
quajoin()
purge_q()
```

Related files are in `utils/`.

---

## Step 11: Implement environment expansion

Expand variables such as:

```bash
echo $USER
echo $HOME
echo $?
echo "$PATH"
```

The expansion process is approximately:

```text
Find $
  ↓
Read variable name
  ↓
Find variable in g_data->env
  ↓
Replace $NAME with its value
  ↓
Handle special variable $?
```

Important functions:

```text
ft_chdollar()
ft_chdollar2()
ft_chdollar3()
ft_transf()
my_getenv()
```

Files:

- `ft_chdollar.c`
- `ft_chdollar2.c`
- `ft_chdollar3.c`

---

## Step 12: Implement the parser

The parser converts a raw string into tokens.

Example:

```bash
cat < input.txt | grep hello > output.txt
```

Possible token sequence:

```text
"cat"
"<"
"input.txt"
"|"
"grep hello"
">"
"output.txt"
```

Main parser functions:

```text
ft_parsing()
ft_parsing2()
tokenize()
ft_tab_to_token()
ft_tab_to_token2()
ft_tokennew()
ft_tokenadd_back()
```

Files:

- `parsing.c`
- `token.c`
- `token_2.c`

The parser must recognize:

```text
Words
Quotes
Pipes
Input redirection
Output redirection
Append redirection
Here-documents
Parentheses
Logical operators
```

---

## Step 13: Implement redirections

### Input redirection

```bash
cat < input.txt
```

Open the file read-only:

```c
open("input.txt", O_RDONLY);
dup2(fd, STDIN_FILENO);
```

### Output redirection

```bash
echo hello > output.txt
```

Open or create the file:

```c
open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);
```

### Append redirection

```bash
echo hello >> output.txt
```

Use:

```c
O_WRONLY | O_CREAT | O_APPEND
```

### Here-document

```bash
cat << EOF
hello
EOF
```

Read lines until the delimiter is entered.

Important functions:

```text
ft_redirect()
ft_file()
ft_chrredirect()
ft_init_redirect()
open_infd()
ft_redirect_3()
here_doc()
ft_hd_finder()
ft_clean_hd()
```

Relevant files:

- `file_redirect.c`
- `file_redirect2.c`
- `file_redirect3.c`
- `here_doc.c`
- `hd.c`

---

## Step 14: Implement pipes

A pipe connects the output of one command to the input of another:

```bash
ls | grep ".c"
```

Flow:

```text
Create pipe()
        ↓
fork first child
        ↓
First child: dup2(pipe[1], STDOUT_FILENO)
        ↓
fork second child
        ↓
Second child: dup2(pipe[0], STDIN_FILENO)
        ↓
Close unused file descriptors
        ↓
Wait for both children
```

Important functions:

```text
exec_pipe()
pipe_ope()
pipex()
first_pipe()
mid_pipe()
last_pipe()
```

Files:

- `pipe.c`
- `exec_2.c`

Test:

```bash
ls | wc -l
cat Makefile | grep CC
echo hello | cat
```

---

## Step 15: Implement logical operators

This version contains logic for:

```bash
command1 && command2
command1 || command2
```

Rules:

```text
A && B
Run B only if A succeeds.

A || B
Run B only if A fails.
```

Example:

```bash
true && echo success
false && echo not_printed
false || echo fallback
```

Important functions:

```text
what_im()
execute()
execute_2()
execute_3()
execute_4()
execute_5()
```

Files:

- `ope.c`
- `ope_2.c`

---

## Step 16: Implement subshells

A subshell groups commands:

```bash
(echo one && echo two)
```

A subshell has its own process and state.

Main function:

```text
subshell()
```

The parser recursively creates child tokens using:

```text
ft_parsing2()
```

The token structures support:

```text
sub_token
sup_token
```

These represent nested commands.

---

## Step 17: Implement wildcard expansion

Wildcard example:

```bash
ls *.c
```

The shell searches the current directory and replaces `*.c` with matching filenames.

Important functions:

```text
ft_chwc()
ft_wcfile()
ft_wcsearch()
ft_chwc_utils()
```

Files:

- `ft_chwc.c`
- `ft_chwc2.c`
- `ft_chwc_utils.c`

Important rule:

```bash
echo "*.c"
```

should not expand the wildcard because it is inside quotes.

---

## Step 18: Add cleanup and memory management

Every allocated object must eventually be freed.

Important cleanup functions:

```text
ft_free()
ft_free_tab()
ft_tokenclean_all()
ft_clean_hd()
ft_memdel()
```

Clean up:

```text
readline() result
temporary strings
argument arrays
tokens
redirect lists
heredoc file descriptors
environment copies
pipes
```

Use tools to check memory:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./minishell
```

Example test:

```bash
printf 'echo hello\nexit\n' | valgrind --leak-check=full ./minishell
```

---

# 5. Function map by folder

## `src/main.c`

```text
main()
initiate_values()
main_2()
main_3()
shlvlhandler()
```

Purpose:

```text
Program startup
Shell initialization
Interactive loop
Environment level management
```

---

## `src/builtins/`

| File | Main functions | Purpose |
|---|---|---|
| `echo.c` | `echo`, `echo_2` | Print arguments |
| `env.c` | `my_setenv`, helpers | Add/update variables |
| `export.c` | `export`, `export_print` | Export variables |
| `unset.c` | `unset`, `del_element` | Remove variables |
| `rep.c` | `cd`, `pwd` | Directory operations |
| `exit.c` | `exit_cmd` | Terminate shell |
| `utils.c` | `print_env`, `format_env` | Built-in helpers |

---

## `src/exec/`

```text
exec.c
    exec()
    lexer()
    cmd_sys()
    not_pid()

exec_2.c
    pipex()
    subshell()

pipe.c
    exec_pipe()
    pipe_ope()

ope.c
    what_im()
    redirect_exec()

ope_2.c
    execute()
    execute_2()
    execute_3()
    execute_4()
    execute_5()
```

Purpose:

```text
Command execution
Built-in selection
fork()
execve()
waitpid()
Pipes
Subshells
Operators
Redirection execution
```

---

## `src/lst/`

### Token list

```text
ft_tokennew()
ft_sub_tokennew()
ft_tokenlast()
ft_tokenadd_back()
ft_token_next()
ft_tokenclean_all()
```

### Command list

```text
ft_cmdnew()
ft_cmdfist()
ft_cmdadd_back()
ft_cmdadd_and()
ft_cmdadd_or()
```

### Heredoc list

```text
ft_hdnew()
ft_hdlast()
ft_hdadd_back()
ft_hdsize()
ft_clean_hd()
```

Purpose:

```text
Build and destroy linked lists used by the parser and executor.
```

---

## `src/signal/`

```text
c_handler()
c_handler_2()
c_handler_fork()
q_handler()
q_handler_fork()
d_handler()
here_doc_sig1()
here_doc_sig2()
```

Purpose:

```text
Ctrl-C
Ctrl-D
Ctrl-\
Signals inside the parent shell
Signals inside child processes
Signals inside heredocs
```

---

## `src/utils/`

The utility functions handle:

```text
Quotes
Variables
Wildcards
Whitespace
Parsing
Redirections
Heredocs
String joining
String replacement
Error messages
Memory cleanup
Token validation
```

Important groups:

| Feature | Functions |
|---|---|
| Whitespace | `ft_epur_str`, `ft_onespace`, `ft_str_isspace` |
| Quote handling | `ft_woquote`, `quajoin`, `purge_q` |
| Variables | `ft_chdollar`, `ft_transf`, `my_getenv` |
| Wildcards | `ft_chwc`, `ft_wcsearch`, `ft_wcfile` |
| Redirection | `ft_redirect`, `ft_file`, `ft_chrredirect` |
| Heredoc | `here_doc`, `ft_hd_finder`, `ft_clean_hd` |
| Parsing | `tokenize`, `ft_pgross_str`, `ft_split2`, `ft_split3`, `ft_split4` |
| Errors | `ft_error`, `puterror`, `ft_token_error` |
| Cleanup | `ft_free`, `ft_free_tab`, `ft_memdel` |

---

# 6. Recommended beginner implementation milestones

Use this order:

```text
Milestone 1: Display prompt
Milestone 2: Read input
Milestone 3: Handle Ctrl-C and Ctrl-D
Milestone 4: Execute one external command
Milestone 5: Implement PATH lookup
Milestone 6: Implement echo
Milestone 7: Implement pwd
Milestone 8: Implement env
Milestone 9: Implement cd
Milestone 10: Implement export
Milestone 11: Implement unset
Milestone 12: Implement exit
Milestone 13: Add single and double quotes
Milestone 14: Add environment expansion
Milestone 15: Add input/output redirection
Milestone 16: Add append redirection
Milestone 17: Add heredoc
Milestone 18: Add pipes
Milestone 19: Add logical operators
Milestone 20: Add subshells
Milestone 21: Add wildcard expansion
Milestone 22: Fix memory leaks
Milestone 23: Test every feature against Bash
```

---

# 7. Test commands for each feature

## Basic commands

```bash
echo hello
pwd
env
ls
```

## Arguments

```bash
echo one two three
ls -la
```

## Quotes

```bash
echo 'hello world'
echo "hello world"
echo '$USER'
echo "$USER"
```

## Variables

```bash
echo $HOME
echo $USER
echo $?
export TEST=hello
echo $TEST
unset TEST
```

## Redirections

```bash
echo hello > file.txt
cat < file.txt
echo again >> file.txt
cat file.txt
```

## Pipes

```bash
ls | wc -l
cat file.txt | grep hello
```

## Heredoc

```bash
cat << EOF
hello
world
EOF
```

## Operators

```bash
true && echo success
false && echo not_printed
false || echo fallback
```

## Signals

```text
Press Ctrl-C
Press Ctrl-D
Press Ctrl-\
```

---

# 8. Important execution example

For this command:

```bash
echo hello | grep hello > result.txt
```

The shell should process it like this:

```text
1. readline()
2. Read the complete input
3. Parse:
   - echo hello
   - |
   - grep hello
   - >
   - result.txt
4. Create a pipe
5. Fork echo process
6. Connect echo stdout to pipe input
7. Fork grep process
8. Connect grep stdin to pipe output
9. Open result.txt
10. Connect grep stdout to result.txt
11. Execute echo
12. Execute grep
13. Wait for both processes
14. Save exit status
15. Close file descriptors
16. Free tokens
```

This is the central idea of the whole project.

---

# 9. How to study this existing project

Read the files in this order:

1. `main.c`
2. `minishell.h`
3. `echo.c`
4. `rep.c`
5. `exec.c`
6. `pipe.c`
7. `parsing.c`
8. `token.c`
9. `ope.c`
10. `ope_2.c`
11. Redirection files.
12. Variable-expansion files.
13. Wildcard files.
14. Signal files.
15. Cleanup functions.

The most important concept is to follow this chain:

```text
main()
  → main_2()
  → main_3()
  → ft_parsing()
  → execute()
  → redirect_exec()
  → exec() or exec_pipe()
  → built-in or execve()
  → cleanup
```

One note: the project `README.md` describes a reduced feature set in some sections, while the current source contains logic for additional features such as `&&`, `||`, and wildcard processing. For learning, follow the behavior implemented in the source code.