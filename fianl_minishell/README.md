*This project has been created as part of the 42 curriculum by vorhansa, ptorsaku.*

# Minishell

## Description

Minishell is an interactive shell written in C. Users can enter commands at a prompt, run built-in commands such as `cd` and `echo`, and connect multiple commands using pipes.
The goal of this project is to understand how a shell interprets and executes commands and manages input and output.

## Instructions

### Requirements and Build

You need `make`, a C toolchain, and the GNU Readline development files. Libft is included in the repository and built automatically by the Makefile.
Run `make` from the root of the repository to build the `minishell` executable.
```sh
make
```
- `make clean` removes the object files.
- `make fclean` removes the object files, `minishell`, and `libft.a`.
- `make re` performs a full rebuild.

### Run and Quick Start

From the root of the repository, start Minishell without arguments:
```sh
./minishell
```
At the `minishell >` prompt, type `echo hello` and press Enter.
The expected output is `hello`.
Press Ctrl+C to clear the current line and get a fresh prompt. Use the Up and Down arrow keys to browse command history.
To leave Minishell, type `exit` or press Ctrl+D at an empty prompt.

## Design and Implementation

### Command Processing

The following example shows how Minishell processes a pipeline.
First, set `DEMO` in Minishell.
```sh
export DEMO="hello world"
```
Then run the pipeline.
```sh
echo "$DEMO" | cat > demo.out
```

1. **Tokenization**: The shell reads the line with GNU Readline. The lexer separates words from operators such as `|` and `>`, keeping the quotes around `"$DEMO"` for expansion.
2. **Validate syntax**: The shell checks the token sequence. The pipe must have a command on each side, and `>` must be followed by a word.
3. **Expand variables**: The shell replaces `"$DEMO"` with `hello world` and removes the double quotes. The result remains a single argument. Double quotes allow $DEMO to expand while keeping its value as one argument. With single quotes, `'$DEMO'` stay literal.
4. **Build commands**: The shell splits the tokens at `|` into two commands. The first is `echo` with `hello world` as its argument. The second is `cat` with `demo.out` as its output file, stored separately from its arguments.

### Command Execution

- Standalone builtins run in the main shell process, so changes made by commands such as `cd` and `export` persist for later commands.
- External programs run in a child process via `execve`. If a command name contains `/`, Minishell uses the supplied path. Otherwise, it searches `PATH` in order.
- Each command in a pipeline runs in its own child process. Changes made by builtins such as `cd` and `export` in those child processes do not affect the main shell.
- In `echo "$DEMO" | cat > demo.out`, the first child runs the `echo` builtin, and the second finds `cat` through `PATH` and runs it with `execve`.
- The pipe sends `echo`'s output to `cat`, whose standard output is redirected to `demo.out`, so the file contains `hello world`. Run `echo $?` immediately after the pipeline to see the exit status of `cat`, the last command in the pipeline.

### Signal Handling

Minishell uses one global variable, `g_signal`, to store a received signal number. For example, when the user presses Ctrl+C at the prompt, the handler records `SIGINT` and uses Readline to clear the current line and redraw the prompt. After Readline returns, the main loop sets the shell status to `130` and resets `g_signal` to `0`. Passing only the signal number lets the main loop update the shell state without giving the handler access to it.

## Resources

### Shell and Readline References

- [POSIX.1-2024: Shell Command Language](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html) — Shell syntax, quoting, expansion, redirection, and command execution.
- [GNU Bash Reference Manual](https://tiswww.case.edu/php/chet/bash/bashref.html) — Reference behavior for pipelines, builtins, signals, and exit status.
- [GNU Readline Manual](https://tiswww.case.edu/php/chet/readline/readline.html) — Interactive line editing, command history, and prompt redisplay.
- [Unix Exit Codes Reference](https://geratools.com/exit-codes-reference) — A quick lookup for exit status conventions.

### Linux Manual Pages

- **Processes and exit status** — [fork(2)](https://man7.org/linux/man-pages/man2/fork.2.html), [execve(2)](https://man7.org/linux/man-pages/man2/execve.2.html), and [waitpid(2)](https://man7.org/linux/man-pages/man2/wait.2.html).
- **Pipes and redirection** — [pipe(7)](https://man7.org/linux/man-pages/man7/pipe.7.html), [dup2(2)](https://man7.org/linux/man-pages/man2/dup.2.html), and [open(2)](https://man7.org/linux/man-pages/man2/open.2.html).
- **Signals and terminals** — [sigaction(2)](https://man7.org/linux/man-pages/man2/sigaction.2.html), [signal-safety(7)](https://man7.org/linux/man-pages/man7/signal-safety.7.html), and [termios(3)](https://man7.org/linux/man-pages/man3/termios.3.html).
- **Working directories** — [chdir(2)](https://man7.org/linux/man-pages/man2/chdir.2.html) and [getcwd(3)](https://man7.org/linux/man-pages/man3/getcwd.3.html).

### Studied Repositories

- [maiadegraaf/minishell](https://github.com/maiadegraaf/minishell) — Splits the token stream at pipes into a linked list of commands and stores redirections separately from arguments.
- [42YerevanProjects/minishell](https://github.com/42YerevanProjects/minishell) — Stores pipeline commands and their child PIDs in arrays indexed by pipeline position.
- [jdecorte-be/minishell](https://github.com/jdecorte-be/minishell) — Stores redirection data on token nodes and uses nested token lists for parenthesized groups.

### AI Usage

We used Codex and GitHub Copilot in two roles.

- A learning companion to explain shell concepts with references, suggest test cases (including edge cases), discuss how the code works, and review it for correctness.
- A research assistant to explore implementation techniques and identify potential optimizations.

We also used AI to translate text and to draft and revise this README.

We wrote the final implementation ourselves. The implementation decisions, code behavior, and final submission remain the authors' responsibility.


## Additional

### Examples

Start `./minishell` from the repository root and enter the commands one at a time. Run commands marked **Bash** in the parent shell. Start a fresh Minishell session if you want to try a section independently. For examples that write files, choose unused filenames in a writable directory.

#### Simple Commands and Global Variables

```sh
/bin/ls
/bin/pwd
/bin/echo
```

Press Enter on an empty line, then try a line containing only spaces or tabs. The shell should return to the prompt each time. If Tab triggers completion, exit Minishell and run this from **Bash** to send the whitespace directly.

```bash
printf '\n   \n  \t  \necho STILL_RUNNING\n' | ./minishell
```

Expect `STILL_RUNNING`. In the source, check how many global variables the shell uses, what `g_signal` stores, and how the main loop uses it after Ctrl+C.

#### Arguments

```sh
/bin/echo alpha beta gamma
/bin/ls -l README.md
wc -l README.md
grep -n Instructions README.md
```

Expect three words, file details, a line count, and matching lines with their line numbers, respectively.

#### `echo`

```sh
echo
echo hello
echo one two three
echo -n hello
echo -n one two three
```

The first command prints a blank line. With `-n`, the next prompt appears on the same line as the output.

#### `exit`

Open a fresh Minishell session for each row. When Minishell exits, run `echo $?` immediately in **Bash**.

| Enter in Minishell | Expected result |
| --- | --- |
| `false`, then `exit` on the next line | Exits with the previous status, `1` |
| `exit 7` | Exits with status `7` |
| `exit -1` | Exits with status `255` |
| `exit abc` | Reports a numeric argument error and exits |
| `exit 1 2` | Reports too many arguments and stays open. Run `echo $?` inside Minishell to check for `1` |

#### Exit Status

```sh
/bin/ls -l README.md
echo $?
grep Instructions README.md
echo $?
grep '^EVAL_NO_MATCH_42$' README.md
echo $?
/bin/ls .eval-missing-42
echo $?
false
expr $? + $?
```

Successful commands return `0`. `grep` with no match returns `1`. Compare the missing-file status with Bash on the same machine. `expr $? + $?` prints `2` because both expansions use the status from `false`.

#### Signals

Try each row separately in a terminal.

| Starting point | Keys | Expected result |
| --- | --- | --- |
| Empty prompt | `Ctrl+C` | Shows a fresh prompt on a new line |
| Empty prompt | `Ctrl+\` | Keeps the prompt active |
| Empty prompt | `Ctrl+D` | Exits Minishell. Reopen it with `./minishell` |
| Type `echo CANCELLED` without pressing Enter | `Ctrl+C`, then Enter | Clears the line before the command can run |
| Type `echo KEEP` with the cursor at the end | `Ctrl+D`, then Enter | Keeps the line. Enter runs `echo KEEP` |
| Type `echo KEEP` without pressing Enter | `Ctrl+\`, then Enter | Keeps the line. Enter runs `echo KEEP` |

Repeat with `cat`, `grep "something"`, and `wc -l`. Start the command again for each key. After returning to the prompt, run `echo $?` immediately.

- `Ctrl+C` should give `130`.
- `Ctrl+\` should give `131`.
- `Ctrl+D` on an empty input line sends EOF. Expect `0` for `cat` and `wc -l`, and `1` for `grep "something"` when no line matched.

#### Double Quotes

```sh
echo "hello   world"
echo "cat lol.c | cat > lol.c"
printf '[%s]\n' "a b" ""
```

Check that the three spaces are preserved and that `|` and `>` print as text. The last command prints `[a b]` and `[]` on separate lines.

#### Single Quotes

```sh
echo '$USER'
echo 'a b | cat > single.out'
printf '[%s]\n' '' 'a b'
```

Check that `$USER`, `|`, and `>` print literally. The last command prints `[]` for the empty argument and `[a b]` for the argument containing a space.

#### `env`

```sh
env
env | grep '^PATH='
```

`env` lists the current environment. The second command shows its `PATH` entry.

#### `export`

```sh
export EVAL_VALUE=first
env | grep '^EVAL_VALUE='
export EVAL_VALUE=second
env | grep '^EVAL_VALUE='
export EVAL_A=one EVAL_B=two
env | grep '^EVAL_[AB]='
```

Check that `EVAL_VALUE` is created and then updated. Both variables from the final `export` should appear in `env`.

#### `unset`

```sh
export EVAL_DROP=one EVAL_KEEP=two
export EVAL_DROP=updated
env | grep '^EVAL_DROP='
unset EVAL_DROP
env | grep '^EVAL_DROP='
echo $?
env | grep '^EVAL_KEEP='
```

Before `unset`, expect `EVAL_DROP=updated`. Afterward, `grep` should find no `EVAL_DROP` entry and return `1`. `EVAL_KEEP=two` should still appear.

#### `cd`

```sh
cd src
/bin/ls
cd .
pwd
cd ..
pwd
cd libft
/bin/ls
cd ..
cd .eval-missing-42
pwd
cd README.md
pwd
```

Check that `cd .` keeps the same directory and that `cd ..` moves up one level. Trying a missing directory or the regular file `README.md` should report an error and leave you at the repository root.

#### `pwd`

```sh
pwd
cd src
pwd
cd ../builtin
pwd
cd ..
```

Expect the absolute path of the repository root, then paths ending in `/src` and `/builtin`.

#### Relative Paths

```sh
export EVAL_START="$PWD"
cd /
./bin/echo ROOT
cd /usr
../bin/echo USR
cd /usr/bin
../../bin/echo DEEP
../../usr/../bin/echo COMPLEX
cd "$EVAL_START"
```

Expect `ROOT`, `USR`, `DEEP`, and `COMPLEX`. The final `cd` returns to the directory where you started.

#### `PATH` Lookup

```sh
ls README.md
wc -l README.md
awk 'END { print NR }' README.md
export EVAL_SAVED_PATH="$PATH"
unset PATH
wc -l README.md
echo $?
/usr/bin/wc -l README.md
export PATH=/bin:/usr/bin
ls README.md
export PATH=/usr/bin:/bin
ls README.md
export PATH="$EVAL_SAVED_PATH"
```

`wc` and `awk` should agree on the line count. With `PATH` unset, `wc` by name should return `127`, and `/usr/bin/wc` should still work.

On Linux, if `strace` is available, start a separate session from **Bash** with `strace -f -e trace=execve ./minishell`. Repeat the two `PATH` orders and the `ls` calls. When both directories contain `ls`, the `execve` trace should show `/bin/ls` first and `/usr/bin/ls` after reversing the order.

#### Redirections

```sh
echo first > .eval-out.txt
printf '%s\n' second >> .eval-out.txt
cat < .eval-out.txt
echo replacement > .eval-out.txt
cat < .eval-out.txt
wc -l < README.md
echo final > .eval-first.txt > .eval-second.txt
wc -c < .eval-first.txt
cat .eval-second.txt
cat < README.md < .eval-out.txt
cat < .eval-missing-42
echo $?
wc -l < .eval-missing-42
echo $?
```

Check that `>>` appends and `>` overwrites. With two output redirections, `.eval-first.txt` should be empty and `.eval-second.txt` should contain `final`. With two input redirections, the last file supplies the input, so `cat < README.md < .eval-out.txt` prints `replacement`. Both missing-file cases should report an error and return a nonzero status.

Enter the heredoc one line at a time. `cat` should print `hello` and `world` after you enter `END`.

```sh
cat << END
hello
world
END
```

#### Pipes

```sh
cat README.md | grep '^## ' | more
printf '%s\n' alpha beta | grep beta | wc -l
wc -l README.md | cat
ls .eval-missing-42 | grep Instructions | wc -l
grep '^## Instructions$' < README.md | cat > .eval-pipe.txt
cat .eval-pipe.txt
```

The first pipeline displays README headings. Press `q` if `more` waits. The second prints `1`, and the third prints the README's line count. The missing-file case reports an error from `ls` and prints `0` from `wc`. The redirected pipeline writes `## Instructions` to `.eval-pipe.txt`.

#### History and Long Input

```sh
echo HISTORY_ONE
echo HISTORY_TWO
```

Use Up and Down to find an earlier command, then press Enter to rerun it. Type `echo CANCELLED` without submitting it, press Ctrl+C, and then press Enter. The old command should never run.

```sh
dsbksdgbksdghsd
echo $?
cat | cat | ls
```

Expect a command-not-found error and status `127`. If the pipeline waits for input, press Ctrl+D on an empty line to let it finish.

Generate a command with 100 arguments in **Bash**, then copy the resulting line into Minishell. Expect `A001` through `A100` in order.

```bash
printf 'echo'
printf ' A%03d' {1..100}
printf '\n'
```

#### Environment Variables

```sh
export USER=evaluser
export DEMO="hello world"
echo $USER
echo "$USER"
echo '$USER'
echo "$USER" "$DEMO"
printf '[%s]\n' "$DEMO"
```

The first two `echo` commands print `evaluser`. The single-quoted example prints `$USER` literally. The final command prints `[hello world]`, showing that the double-quoted value stays together as one argument.

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

### In team code

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

Team code stores the same current terminal configuration twice:

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