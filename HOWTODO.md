A good way to split this project between two people is by **responsibility**, not by randomly dividing files.

## Recommended division

### Person 1: Input, parsing, and expansion

Responsible for converting the user’s command line into structured commands.

Files/modules:

```text
src/utils/
src/lst/token.c
src/lst/token_2.c
src/lst/cmd.c
src/lst/hd.c
src/lst/hd_2.c
```

Main responsibilities:

1. Read and validate the input.
2. Handle quotes:
   - Single quotes
   - Double quotes
3. Expand environment variables:
   - `$USER`
   - `$HOME`
   - `$PATH`
   - `$?`
4. Detect syntax errors.
5. Tokenize the command.
6. Parse:
   - Commands
   - Arguments
   - Pipes
   - Redirections
7. Build the `t_token` or `t_cmd` linked list.
8. Parse here-document delimiters.
9. Free parser-related data.

Person 1 should provide a clean result such as:

```c
t_token *parse_line(char *line);
```

The execution code should not need to understand how the original string was parsed.

---

### Person 2: Execution, built-ins, processes, and signals

Responsible for executing the parsed command structure.

Files/modules:

```text
src/exec/
src/builtins/
src/signal/
```

Main responsibilities:

1. Execute external commands using:
   - `fork()`
   - `execve()`
   - `waitpid()`
2. Search for executables through `$PATH`.
3. Implement pipes.
4. Implement redirections:
   - `>`
   - `>>`
   - `<`
   - `<<`
5. Implement built-ins:
   - `echo`
   - `cd`
   - `pwd`
   - `export`
   - `unset`
   - `env`
   - `exit`
6. Handle signals:
   - `SIGINT`
   - `SIGQUIT`
7. Track exit statuses.
8. Handle terminal settings.
9. Free execution-related file descriptors and processes.

Person 2 should provide an execution interface such as:

```c
int execute(t_token *tokens);
```

---

## Shared responsibility

Some files should be designed together before coding:

```text
includes/minishell.h
src/main.c
Makefile
```

These files define the connection between both parts.

### `minishell.h`

Agree on the structures before starting:

```c
typedef struct s_token
{
    char            *cmd;
    char            **args;
    int             input_fd;
    int             output_fd;
    struct s_token  *next;
} t_token;
```

The exact structure can be different, but both people must agree on:

- What each field means
- Who allocates it
- Who frees it
- Which values indicate errors
- How pipes and redirections are represented

### `main.c`

One person should own the main loop to avoid conflicts. Usually Person 1 can implement input and parsing, while Person 2 provides `execute()`.

The main loop should eventually look like:

```c
while (1)
{
    line = readline(prompt());

    if (!line)
        break;

    if (syntax_error(line))
    {
        free(line);
        continue;
    }

    tokens = parse_line(line);
    execute(tokens);

    free_tokens(tokens);
    free(line);
}
```

### `Makefile`

Agree on the Makefile at the beginning. Both people should be able to compile the whole project locally.

---

# Suggested development order

## Phase 1: Agree on the architecture

Before writing implementation code, decide:

1. The required features.
2. The token structure.
3. The environment representation.
4. The parser-to-executor interface.
5. Memory ownership rules.
6. Exit-status behavior.
7. Which person owns each file.

For example:

```text
parse_line()  ->  t_token list  ->  execute()
```

This interface should not change frequently.

---

## Phase 2: Build a minimal shell together

First create a working skeleton:

```text
readline()
    ↓
split command into arguments
    ↓
fork()
    ↓
execve()
    ↓
waitpid()
```

At this stage, support only:

```bash
ls
pwd
echo hello
```

This ensures that the project compiles and both people understand the complete flow.

---

## Phase 3: Work independently

### Person 1 implements

In this order:

1. Basic tokenizer
2. Whitespace handling
3. Quote handling
4. Syntax validation
5. Environment expansion
6. Pipe parsing
7. Redirection parsing
8. Here-document parsing
9. Parser cleanup functions

### Person 2 implements

In this order:

1. External command execution
2. `$PATH` lookup
3. Basic built-ins
4. Parent-process built-ins
5. Pipes
6. File redirections
7. Here-document execution
8. Signals
9. Exit-status handling
10. Process cleanup

---

# Important dependency order

Some features depend on both people’s work:

```text
Basic tokenizer
    ↓
Command structure
    ↓
Basic external execution
    ↓
Built-ins
    ↓
Pipes
    ↓
Redirections
    ↓
Here-documents
    ↓
Signals and edge cases
```

Do not implement complex pipelines before agreeing on the token structure.

---

# Git workflow

Use a shared repository with separate branches:

```text
main
├── feature/parser
└── feature/execution
```

Person 1 works on:

```bash
git checkout -b feature/parser
```

Person 2 works on:

```bash
git checkout -b feature/execution
```

Commit small, focused changes:

```bash
git add src/utils
git commit -m "Add command tokenizer"
```

```bash
git add src/exec
git commit -m "Add external command execution"
```

Then merge through pull requests:

```text
feature/parser
    ↓
main

feature/execution
    ↓
main
```

Do not work directly on `main`.

---

# Avoid merge conflicts

The highest-conflict files will be:

```text
includes/minishell.h
src/main.c
Makefile
```

To reduce conflicts:

1. Design `minishell.h` together before coding.
2. Let only one person edit `main.c`.
3. Let only one person edit the Makefile.
4. Put new code in separate source files.
5. Do not reformat files belonging to the other person.
6. Merge changes frequently.
7. Tell the other person before changing a shared structure.

---

# Define ownership clearly

Use a table like this:

| Area | Owner |
|---|---|
| Main loop | Person 1 |
| Readline and prompt | Person 1 |
| Syntax validation | Person 1 |
| Quotes | Person 1 |
| Variable expansion | Person 1 |
| Tokenization | Person 1 |
| External execution | Person 2 |
| Pipes | Person 2 |
| Redirections | Person 2 |
| Built-ins | Person 2 |
| Signals | Person 2 |
| Exit status | Person 2 |
| Header design | Both |
| Makefile | One designated owner |
| Integration testing | Both |
| Final memory-leak testing | Both |

---

# Testing responsibilities

Both people should test their own modules.

## Person 1 tests parsing

```bash
echo hello
echo "hello world"
echo '$USER'
echo "$USER"
echo $?
echo hello | cat
echo hello > file
```

Check that the generated token structure is correct.

## Person 2 tests execution

```bash
pwd
cd /tmp
export TEST=hello
echo $TEST
unset TEST
ls | wc -l
echo hello > file
cat < file
cat << EOF
hello
EOF
```

Check:

- Output
- Exit code
- File descriptors
- Child processes
- Memory leaks

## Both test integration

```bash
echo hello | grep hello > output
cat << EOF | grep hello
hello
world
EOF
```

---

# Recommended milestones

## Milestone 1: Basic shell

Both people work together:

```bash
echo hello
pwd
ls
```

## Milestone 2: Parser and executor interface

Person 1 creates the token structure. Person 2 executes it.

```text
input → tokens → execute
```

## Milestone 3: Built-ins and variables

Add:

```bash
cd
export
unset
env
echo $USER
```

## Milestone 4: Pipes

Add:

```bash
ls | grep .c | wc -l
```

## Milestone 5: Redirections

Add:

```bash
echo hello > file
cat < file
echo again >> file
```

## Milestone 6: Here-documents and signals

Add:

```bash
cat << EOF
hello
EOF
```

Then test `Ctrl-C`, `Ctrl-D`, and `Ctrl-\`.

## Milestone 7: Cleanup and testing

Together:

- Run compiler warnings.
- Run memory-leak checks.
- Test invalid syntax.
- Compare behavior with Bash.
- Test long pipelines.
- Test commands with spaces and quotes.
- Check for zombie processes.
- Check for file-descriptor leaks.

---

## Best final architecture

```text
main.c
  |
  |-- readline and prompt
  |
  |-- parser
  |     |-- quote handling
  |     |-- variable expansion
  |     |-- tokenization
  |     |-- syntax validation
  |
  |-- executor
        |-- built-ins
        |-- external commands
        |-- pipes
        |-- redirections
        |-- here-documents
        |-- signals
```

The most important rule is:

> Person 1 produces a reliable command structure. Person 2 consumes that structure and executes it.

That gives both people independent work while keeping the integration point clear.