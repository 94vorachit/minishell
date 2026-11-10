This `init` is used to initialize the basic state of your minishell before command parsing and execution.

## 1. Global shell data

```c
t_data *g_data;
```

This is a global pointer containing shell information such as:

- Environment variables
- Last exit status
- Command count

It is allocated in `main()`.

---

## 2. `update_shlvl()`

```c
static int	update_shlvl(t_env *env, int shlvl)
```

This updates the `SHLVL` environment variable.

For example:

```text
SHLVL=2
```

becomes:

```text
SHLVL=3
```

### What it does

```c
level = ft_itoa(shlvl);
```

Converts the integer to a string:

```text
3 -> "3"
```

```c
new_value = ft_strjoin("SHLVL=", level);
```

Creates:

```text
"SHLVL=3"
```

```c
free(level);
```

Releases the temporary string created by `ft_itoa()`.

```c
free(env->value);
env->value = new_value;
```

Releases the old value and replaces it with the new one.

It returns:

- `1` when the update succeeds
- `0` when allocation fails

---

## 3. `ft_shlvl()`

```c
void	ft_shlvl(t_env *env)
```

This searches the copied environment list for the exact variable:

```text
SHLVL=
```

```c
val = getenv("SHLVL");
```

Reads the current process environment’s `SHLVL`.

```c
if (!val || val[0] == '\0')
	return ;
```

Stops safely when:

- `SHLVL` does not exist
- `SHLVL` is empty

```c
shlvl = ft_atoi(val) + 1;
```

Converts the current value to an integer and increments it.

```c
if (ft_strncmp("SHLVL=", env->value, 6) == 0)
```

Checks for the exact prefix `SHLVL=`.

This avoids incorrectly matching:

```text
SHLVL_EXTRA=5
```

Once the variable is found, `update_shlvl()` changes its value and the function returns.

---

## 4. `ft_lstadd_val()`

```c
t_env *ft_lstadd_val(char *value, t_env *next)
```

Creates one linked-list node for an environment variable.

For example:

```text
value = "PATH=/bin"
next = another node
```

The list looks like:

```text
[PATH=/bin] -> [HOME=/home/user] -> NULL
```

It allocates memory for the node and stores:

```c
new->value = value;
new->next = next;
```

---

## 5. `ft_init_env()`

```c
static int	ft_init_env(t_env **mini_env, char **env)
```

Copies the environment array into a linked list.

The original `env` looks like:

```c
env[0] = "PATH=/bin";
env[1] = "HOME=/home/user";
env[2] = "SHLVL=2";
env[3] = NULL;
```

This function creates:

```text
[PATH=/bin] -> [HOME=/home/user] -> [SHLVL=2] -> NULL
```

### Empty environment handling

```c
*mini_env = NULL;
if (!env || !env[0])
	return (0);
```

This safely handles:

```c
env == NULL
```

or:

```c
env[0] == NULL
```

### Node creation

```c
temp = ft_lstadd_val(ft_strdup(env[i++]), NULL);
```

Duplicates the environment string and stores it in a new linked-list node.

### Additional variables

```c
while (env[i])
```

Continues copying each environment entry.

```c
if (!temp->next)
	return (-1);
```

Checks whether allocation of the next node failed.

It returns:

- `0` on success
- `-1` on allocation failure

---

## 6. `init()`

```c
int	init(t_data *mini, char **env)
```

Initializes the main shell structure.

```c
mini->status = 0;
mini->cmd_count = 0;
```

Sets the initial shell state:

- Last command status: `0`
- Number of commands: `0`

```c
if (ft_init_env(&mini->env, env) == -1)
	return (-1);
```

Copies the process environment into the shell’s linked-list environment.

```c
ft_shlvl(mini->env);
```

Increments `SHLVL`.

---

## 7. `main()`

```c
int	main(int ac, char **av, char **env)
```

This is the program entry point.

The parameters are:

- `ac`: argument count
- `av`: argument values
- `env`: environment variables

### Ignore `av`

```c
(void)av;
```

This prevents an unused-parameter warning.

### Reject arguments

```c
if (ac != 1)
```

The minishell should be started without arguments:

```bash
./minishell
```

If arguments are provided, it prints an error.

### Allocate shell state

```c
g_data = malloc(sizeof(*g_data));
```

Allocates memory for the global shell data.

```c
if (!g_data)
	return (1);
```

Stops if allocation fails.

### Initialize shell data

```c
if (init(g_data, env) == -1)
```

Builds the environment list and initializes shell values.

### Successful exit

```c
return (0);
```

Returns success.

---

## Overall flow

```text
main()
  |
  |-- Check command-line arguments
  |
  |-- Allocate g_data
  |
  |-- init()
        |
        |-- Set status and command count
        |
        |-- Copy env[] into linked list
        |
        |-- Increment SHLVL
```

At the moment, this code only initializes the shell. It does **not yet**:

- Display a prompt
- Read commands with `readline()`
- Parse commands
- Execute commands
- Handle pipes
- Handle redirections
- Free the environment list before exiting

Those features would normally be added after `init()` in the main shell loop.