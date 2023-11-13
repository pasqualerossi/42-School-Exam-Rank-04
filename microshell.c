#include "microshell.h"

int ft_print(char *message, char *av)
{
    int i = 0;
    while (message[i])
    {
        write(2, &message[i], 1);
        i++;
    }
    if (av)
    {
        i = 0;
        while (av[i])
        {
            write(2, &av[i], 1);
            i++;
        }
    }
    write(2, "\n", 1);
    return (1);
}

void ft_cd(char **av, int j)
{
    if (j != 2)
        ft_print("error: cd: bad arguments", 0);
    else if (chdir(av[1]) == -1)
        ft_print("error: cd: cannot change ", av[1]);
}

void ft_exec(char **av, char **env, int j)
{
    int fd[2];
    int has_pipe = (av[j]) && (strcmp(av[j], "|") == 0);
    if ((has_pipe) && (pipe(fd) == -1))
    {
        ft_print("error: fatal", 0);
        return;
    }
    int status;
    int pid = fork();
    if (pid == 0)
    {
        av[j] = 0;
        if ((has_pipe) && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
        {
            ft_print("error: fatal", 0);
            return;
        }
        execve(*av, av, env);
        ft_print("error: cannot execute ", av[0]);
        return ;
    }
    waitpid(pid, &status, 0);
    if ((has_pipe) && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
    {
        ft_print("error: fatal", 0);
    }
}
int main(int ac, char **av, char **env)
{
    if (ac > 1)
    {
        int i = 1;
        int j = 0;
        while (ac > i)
        {
            j = 0;
            while(av[i + j] && (strcmp(av[i + j], "|") && strcmp(av[i + j], ";")))
                j++;
            if (strcmp(av[i], "cd") == 0)
                ft_cd(&av[i], j);
            else if (j)
                ft_exec(&av[i], env, j);
            i += j + 1;
        }
    }
}
