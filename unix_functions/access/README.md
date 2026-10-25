### add_history()

In this code, `access()` is a POSIX function used to check whether a file exists and whether the current process has certain permissions.

```c
access(path, mode)
```

In your code:

```c
if (access(exec, F_OK | X_OK) == 0)
```

- `exec` is the file path being checked.
- `F_OK` checks whether the file exists.
- `X_OK` checks whether the file is executable.
- Return value `0` means the check succeeded.
- Return value `-1` means the check failed.

So this code searches each directory in `$PATH` and returns the first executable matching the command:

```c
/path/from/PATH/command
```

For example, for `ls`, it may test:

```c
/bin/ls
/usr/bin/ls
```

until `access()` finds an existing executable file.

The header normally required for `access()` is:

```c
#include <unistd.h>
```

Note that `access()` checks permissions using the real user ID, while `execve()` uses the effective user ID. For a normal minishell this is usually acceptable, but it can matter in set-user-ID programs.

Here are some examples of using `access()`.

### 1. Check if a file exists

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    if (access("file.txt", F_OK) == 0)
        printf("file.txt exists\n");
    else
        printf("file.txt does not exist\n");

    return 0;
}
```

`F_OK` only checks existence.

---

### 2. Check if a file is readable

```c
if (access("file.txt", R_OK) == 0)
    printf("The file can be read\n");
else
    printf("The file cannot be read\n");
```

---

### 3. Check if a file is writable

```c
if (access("file.txt", W_OK) == 0)
    printf("The file can be modified\n");
else
    printf("The file cannot be modified\n");
```

---

### 4. Check if a file is executable

```c
if (access("my_program", X_OK) == 0)
    printf("The file is executable\n");
else
    printf("The file is not executable\n");
```

---

### 5. Check multiple permissions

```c
if (access("script.sh", F_OK | X_OK) == 0)
    printf("script.sh exists and is executable\n");
else
    printf("script.sh does not exist or is not executable\n");
```

---

### 6. Example related to your minishell

Suppose the user enters:

```bash
ls
```

Your shell searches directories from `$PATH`, for example:

```text
/bin
/usr/bin
```

Then it constructs paths:

```c
exec = "/bin/ls";
```

And checks them:

```c
if (access(exec, F_OK | X_OK) == 0)
{
    printf("Found executable: %s\n", exec);
    return (exec);
}
```

If `/bin/ls` exists and is executable, `access()` returns `0`.

Your `access_path()` function is effectively doing this:

```c
char *access_path(char **allpath, char **s_cmd, char *cmd)
{
    int i;
    char *path;
    char *full_path;

    i = 0;
    while (allpath[i])
    {
        path = ft_strjoin(allpath[i], "/");
        full_path = ft_strjoin(path, s_cmd[0]);
        free(path);

        if (access(full_path, F_OK | X_OK) == 0)
            return (full_path);

        free(full_path);
        i++;
    }
    return (cmd);
}
```

For the command `cat`, the checks might be:

```text
/bin/cat       -> found
/usr/bin/cat   -> not checked
```

For a nonexistent command:

```bash
unknown_command
```

The checks fail:

```text
/bin/unknown_command       -> fails
/usr/bin/unknown_command   -> fails
```

Then the function returns the original command, and your shell can display something like:

```text
unknown_command: command not found
```