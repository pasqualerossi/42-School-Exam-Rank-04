#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int ft_putstr(char *str, char *arg)
{
	while (*str)
		write(2, str++, 1);
	while (arg && *arg)
		write(2, arg++, 1);
	write(2, "\n", 1);
	return (1);
}

int ft_exec(char **av, int end, int tmp, char **env)
{
	av[end] = NULL;
	dup2(tmp, STDIN_FILENO);
	close(tmp);
	execve(av[0], av, env);
	return (ft_putstr("error: cannot execute ", av[0]));
}

int	main(int ac, char **av, char **env)
{
	(void)ac;
	int i = 0;
	int tmp = dup(STDIN_FILENO);

	while (av[i] && av[i + 1])
	{
		av = &av[i + 1];
		i = 0;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		if (!strcmp(av[0], "cd"))
		{
			if (i != 2)
				ft_putstr("error: cd: bad arguments", NULL);
			else if (chdir(av[1]))
				ft_putstr("error: cd: cannot change directory to ", av[1]);
		}
		else if (i && (av[i] == NULL || !strcmp(av[i], ";")))
		{
			int pid = fork();
			if (!pid)
			{
				if (ft_exec(av, i, tmp, env))
					return (1);
			}
			else
			{
				close(tmp);
				while (waitpid(-1, NULL, WUNTRACED) != -1)
					;
				tmp = dup(STDIN_FILENO);
			}
		}
		else if (i && !strcmp(av[i], "|"))
		{
			int fd[2];
			pipe(fd);
			int pid = fork();
			if (!pid)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				if (ft_exec(av, i, tmp, env))
					return (1);
			}
			else
			{
				close(fd[1]);
				close(tmp);
				tmp = fd[0];
			}
		}
	}
	close(tmp);
	return (0);
}
