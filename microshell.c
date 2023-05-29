#include "microshell.h"

int write_error(char *string, char *argv) 
{
    while (*string)
        write(2, string++, 1);
    if (argv)
        while (*argv)
            write(2, argv++, 1);
    write(2, "\n", 1);
    return 1;
}

int ft_exe(char **argv, int i, int tmp_fd, char **env) 
{
    argv[i] = NULL;
    dup2(tmp_fd, STDIN_FILENO);
    close(tmp_fd);
    execve(argv[0], argv, env);
    return write_error("error: cannot execute ", argv[0]);
}

void execute_commands(char **argv, int tmp_fd, char **env) 
{
    int i = 0;
    int fd[2];

    while (argv[i] && argv[i + 1]) 
    {
        argv = &argv[i + 1];
        i = 0;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
            i++;
        if (strcmp(argv[0], "cd") == 0) 
        {
            if (i != 2)
                write_error("error: cd: bad arguments", NULL);
            else if (chdir(argv[1]) != 0)
                write_error("error: cd: cannot change directory to ", argv[1]);
        } 
        else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0)) 
        {
            if (fork() == 0) 
            {
                if (ft_exe(argv, i, tmp_fd, env))
                    _exit(1);
            } 
            else 
            {
                close(tmp_fd);
                while (waitpid(-1, NULL, WUNTRACED) != -1)
                    tmp_fd = dup(STDIN_FILENO);
            }
        } 
        else if (i != 0 && strcmp(argv[i], "|") == 0) 
        {
            pipe(fd);
            if (fork() == 0) 
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                if (ft_exe(argv, i, tmp_fd, env))
                    _exit(1);
            } 
            else 
            {
                close(fd[1]);
                close(tmp_fd);
                tmp_fd = fd[0];
            }
        }
    }
    close(tmp_fd);
}

int main(int argc, char **argv, char **env) 
{
    int tmp_fd = dup(STDIN_FILENO);
    (void)argc;
    execute_commands(argv, tmp_fd, env);
    return 0;
}
