#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int fd;

int err(char *s)
{
	while (*s)
		write(2, s++, 1);
	return 1;
}

int cdir(char **av, int i)
{
	if (i != 2)
		return err("error: cd: bad arguments\n");
	if (chdir(av[1]))
		return err("error: cd: cannot change directory to ") & err(av[1]) & err("\n");
	return 0;
}

int ex(char **av, char **envp, int i)
{
	int fds[2];
	int	res;
	int pip = (av[i] && !strcmp(av[i], "|"));
	
	if (pip && (pipe(fds)))
		return err("error: fatal\n");
	int pid = fork();
	if (!pid)
	{
		av[i] = 0;
		if (dup2(fd, 0) == -1 | close(fd) == -1 | (pip && (dup2(fds[1], 1)
			== -1 | close(fds[0]) == -1 | close(fds[1]) == -1)))
			return err("error: fatal\n");
		execve(*av, av, envp);
		return err("error: cannot execute ") & err(*av) & err("\n");
	}
	if ((pip && (dup2(fds[0], fd) == -1 | close(fds[0]) == -1
		| close(fds[1]) == -1)) | (!pip && dup2(0, fd) == -1) |
		waitpid(pid, &res, 0) == -1)
		return err("error: fatal\n");
	return WIFEXITED(res) && WEXITSTATUS(res);
}

int main(int ac, char **av, char **envp)
{
	(void)ac;
	int i = 0;
	int j = 0;

	fd = dup(0);
	while (av[i] && av[++i])
	{
		av = av + i;
		i = 0;
		while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			i++;
		if (!strcmp(*av, "cd"))
			j = cdir(av, i);
		else if (i)
			j = ex(av, envp, i);
	}
	return ((dup2(0, fd) == -1) && err("error: fatal\n")) | j;
}
