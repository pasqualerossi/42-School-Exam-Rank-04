#include <string.h>
#include <unistd.h>
#include <wait.h>

int tmp;

int p(char *s)
{
	while (*s)
		write(2, s++, 1);
	return 1;
}

int c(char **av, int i)
{
	if (i != 2)
		return p("error: cd: bad arguments\n");
	if (chdir(av[1]))
		return p("error: cd: cannot change directory to ") & p(av[1]) & p("\n");
	return 0;
}

int x(char **av, char **ep, int i)
{
	int tm2[2], r;
	int pip = (av[i] && !strcmp(av[i], "|"));
	
	if (pip && (pipe(tm2)))
		return p("error: fatal\n");
	int pid = fork();
	if (!pid)
	{
		av[i] = 0;
		if (dup2(tmp, 0) == -1 | close(tmp) == -1 | (pip && (dup2(tm2[1], 1)
			== -1 | close(tm2[0]) == -1 | close(tm2[1]) == -1)))
			return p("error: fatal\n");
		execve(*av, av, ep);
		return p("error: cannot execute ") & p(*av) & p("\n");
	}
	if ((pip && (dup2(tm2[0], tmp) == -1 | close(tm2[0]) == -1
		| close(tm2[1]) == -1)) | (!pip && dup2(0, tmp) == -1) |
		waitpid(pid, &r, 0) == -1)
		return p("error: fatal\n");
	return WIFEXITED(r) && WEXITSTATUS(r);
}

int main(int ac, char **av, char **ep)
{
	(void)ac;
	int i = 0, r = 0;
	tmp = dup(0);
	
	while (av[i] && av[++i])
	{
		av = av + i;
		i = 0;
		while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			i++;
		if (!strcmp(*av, "cd"))
			r = c(av, i);
		else if (i)
			r = x(av, ep, i);
	}
	return ((dup2(0, tmp) == -1) && p("error: fatal\n")) | r;
}
