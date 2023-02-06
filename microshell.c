#include "microshell.h"

int	write_error(char *string, char *argc)
{
	while (*string)
		write(2, string++, 1);
	if (argc)
		while (*argc)
			write(2, argc++, 1);
	write(2, "\n", 1);
	return (1);
}

int	ft_execute(char **argv, int i, int temporary, char **environment)
{
	argv[i] = NULL;
	dup2(temporary, 0);
	close(temporary);
	execve(argv[0], argv, environment);
	return (write_error("error: cannot execute ", argv[0]));
}

int	main(int argc, char **argv, char **environment)
{
	int i = 0;
	int temporary_file_descriptor = dup(0);
	int file_descriptor[2];
	(void)argc;

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
				if (ft_execute(argv, i, temporary_file_descriptor, environment))
					return (1);
			}
			else
			{
				close(temporary_file_descriptor);
				while (waitpid(-1, NULL, 0) != -1)
					;
				temporary_file_descriptor = dup(0);
			}
		}
		else if (i != 0 && strcmp(argv[i], "|") == 0)
		{
			pipe(file_descriptor);
			if (fork() == 0)
			{
				dup2(file_descriptor[1], 1);
				close(file_descriptor[0]);
				close(file_descriptor[1]);
				if (ft_execute(argv, i, temporary_file_descriptor, environment))
					return (1);
			}
			else
			{
				close(file_descriptor[1]);
				close(temporary_file_descriptor);
				temporary_file_descriptor = file_descriptor[0];
			}
		}
	}
	close(temporary_file_descriptor);
	return (0);
}
