#include "microshell.h"

int	write_error(char *string, char *argv)
{
	while (string && *string)
		write(2, string++, 1);
	if (argv)
		while(*argv)
			write(2, argv++, 1);
	write(2, "\n", 1);
	return (1);
}

int	ft_exe(char **argv, int i, int temporary_file_descriptor, char **environment_variables)
{
	argv[i] = NULL;
	dup2(temporary_file_descriptor, STDIN_FILENO);
	close(temporary_file_descriptor);
	execve(argv[0], argv, environment_variables);
	return (write_error("error: cannot execute ", argv[0]));
}


int	main(int argc, char **argv, char **environment_variables)
{
	int i = 0;
	int fd[2];
	int temporary_file_descriptor = dup(STDIN_FILENO);
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
				write_error("error: cd: cannot change directory to ", argv[1]	);
		}
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
		{
			if ( fork() == 0)
			{
				if (ft_exe(argv, i, temporary_file_descriptor, environment_variables))
					return (1);
			}
			else
			{
				close(temporary_file_descriptor);
				while(waitpid(-1, NULL, WUNTRACED) != -1)
				temporary_file_descriptor = dup(STDIN_FILENO);
			}
		}
		else if (i != 0 && strcmp(argv[i], "|") == 0)
		{
			pipe(fd);
			if ( fork() == 0)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				if (ft_exe(argv, i, temporary_file_descriptor, environment_variables))
					return (1);
			}
			else
			{
				close(fd[1]);
				close(temporary_file_descriptor);
				temporary_file_descriptor = fd[0];
			}
		}
	}
	close(temporary_file_descriptor);
	return (0);
}
