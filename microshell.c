#include <libc.h>

int	declare(char *str, char *arg)
{
	while (*str)
		write(2, str++, 1);
	if (arg)
		while(*arg)
			write(2, arg++, 1);
	return (write(2, "\n", 1), 1);
}

int ft_ex(char *av[], int i, int TMP, char *env[])
{
	av[i] = NULL;
	dup2(TMP, 0);
	close(TMP);
	execve(av[0], av, env);
	return (declare("error: cannot execute ", av[0]));
}

int	main(int ac, char *av[], char *env[])
{
	int	i, fd[2], TMP;
	(void)ac;

	i = 0;
	TMP = dup(0);
	
	while (av[i] && av[i + 1]) 
	{
		av = &av[i + 1];
		i = 0;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		if (!strcmp(av[0], "cd")) 
		{
			if (i != 2)
				declare("error: cd: bad arguments", NULL);
			else if (chdir(av[1]) != 0)
				declare("error: cd: cannot change directory to ", av[1]);
		}
		else if  (i != 0 && (av[i] == NULL || !strcmp(av[i], ";"))) 
		{
			if (!fork()) 
			{
				if (ft_ex(av, i, TMP, env))
					return (1);
			}
			else 
			{
				close(TMP);
				while(waitpid(-1, NULL, 0) != -1);
			}
			TMP = dup(0);
		}
		else if (i != 0 && !strcmp(av[i], "|")) 
		{
			pipe(fd);
			if (!fork()) 
			{
				dup2(fd[1], 1);
				close(fd[0]), close(fd[1]);
				if (ft_ex(av, i, TMP, env))
					return (1);
			}
			else 
			{
				close(fd[1]), close(TMP);
				TMP = fd[0];
			}
		}
	}
	return (close(TMP), 0);
}
