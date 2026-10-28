/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vorhansa <vorhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 19:36:16 by vorhansa          #+#    #+#             */
/*   Updated: 2026/10/28 20:14:53 by vorhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	free_split(char **arr)
{
    int	i;

    if (!arr)
        return ;
    i = 0;
    while (arr[i])
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}

void	free_cmd(t_cmd *cmd)
{
    int	i;

    if (!cmd)
        return ;
    if (cmd->argv)
    {
        i = 0;
        while (cmd->argv[i])
        {
            free(cmd->argv[i]);
            i++;
        }
        free(cmd->argv);
        cmd->argv = NULL;
    }
    free(cmd->infile);
    free(cmd->outfile);
    cmd->infile = NULL;
    cmd->outfile = NULL;
    cmd->append = 0;
}

char	**env_to_char_array(t_env *env)
{
    char	**arr;
    int		count;
    int		i;

    count = 0;
    while (env)
    {
        count++;
        env = env->next;
    }
    arr = malloc(sizeof(char *) * (count + 1));
    if (!arr)
        return (NULL);
    i = 0;
    env = g_data->env;
    while (env)
    {
        arr[i++] = ft_strdup(env->value);
        env = env->next;
    }
    arr[i] = NULL;
    return (arr);
}

char	*find_executable_path(t_env *env, const char *cmd)
{
    char	*path_env;
    char	**paths;
    char	*tmp;
    char	*full;
    int		i;

    if (!cmd || cmd[0] == '\0')
        return (NULL);
    if (ft_strchr(cmd, '/'))
        return (ft_strdup(cmd));
    path_env = env_get(env, "PATH");
    if (!path_env || !path_env[0])
        return (NULL);
    paths = ft_split(path_env, ':');
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        full = ft_strjoin(paths[i], "/");
        tmp = ft_strjoin(full, cmd);
        free(full);
        if (access(tmp, X_OK) == 0)
        {
            free_split(paths);
            return (tmp);
        }
        free(tmp);
        i++;
    }
    free_split(paths);
    return (NULL);
}

int	build_simple_cmd(t_token *tokens, t_cmd *cmd)
{
    t_token	*cur;
    int		argc;
    int		i;

    if (!cmd)
        return (-1);
    ft_bzero(cmd, sizeof(*cmd));
    cur = tokens;
    argc = 0;
    while (cur && cur->type != T_PIPE)
    {
        if (cur->type == T_WORD)
            argc++;
        else if (ft_isredirection(cur->type))
        {
            if (!cur->next || cur->next->type != T_WORD)
                return (-1);
            if (cur->type == T_REDIR_IN)
                cmd->infile = ft_strdup(cur->next->value);
            else if (cur->type == T_REDIR_OUT)
                cmd->outfile = ft_strdup(cur->next->value);
            else if (cur->type == T_APPEND)
            {
                cmd->outfile = ft_strdup(cur->next->value);
                cmd->append = 1;
            }
            cur = cur->next;
        }
        cur = cur->next;
    }
    cmd->argv = malloc(sizeof(char *) * (argc + 1));
    if (!cmd->argv)
        return (-1);
    i = 0;
    cur = tokens;
    while (cur && cur->type != T_PIPE)
    {
        if (cur->type == T_WORD)
            cmd->argv[i++] = ft_strdup(cur->value);
        else if (ft_isredirection(cur->type))
            cur = cur->next;
        cur = cur->next;
    }
    cmd->argv[i] = NULL;
    return (0);
}

int	execute_simple_cmd(t_data *data, t_cmd *cmd)
{
    pid_t	pid;
    int		status;
    char	*exec_path;
    char	**envp;
    int		fd;

    if (!data || !cmd || !cmd->argv || !cmd->argv[0])
        return (SUCCESS);

    exec_path = find_executable_path(data->env, cmd->argv[0]);
    if (!exec_path)
    {
        ft_putstr_fd("minishell: command not found: ", STDERR);
        ft_putstr_fd(cmd->argv[0], STDERR);
        ft_putstr_fd("\n", STDERR);
        data->status = 127;
        return (127);
    }

    envp = env_to_char_array(data->env);
    if (!envp)
    {
        free(exec_path);
        data->status = 1;
        return (ERROR);
    }

    pid = fork();
    if (pid == 0)
    {
        if (cmd->infile)
        {
            fd = open(cmd->infile, O_RDONLY);
            if (fd < 0)
            {
                perror(cmd->infile);
                free(exec_path);
                free_split(envp);
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (cmd->outfile)
        {
            fd = open(cmd->outfile,
                    O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC),
                    0644);
            if (fd < 0)
            {
                perror(cmd->outfile);
                free(exec_path);
                free_split(envp);
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execve(exec_path, cmd->argv, envp);
        perror("execve");
        free(exec_path);
        free_split(envp);
        exit(127);
    }
    waitpid(pid, &status, 0);

    free(exec_path);
    free_split(envp);

    if (WIFEXITED(status))
        data->status = WEXITSTATUS(status);
    else
        data->status = 128 + WTERMSIG(status);
    return (SUCCESS);
}
