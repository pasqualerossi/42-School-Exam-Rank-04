#ifndef MICROSHELL_H
# define MICROSHELL_H

#include <string.h> // strcmp
#include <unistd.h> // write, dup, dup2, close, execve, chdir and fork
#include <sys/wait.h> // waitpid

int write_error(char *string, char *argv);
int	ft_exe(char **argv, int i, int temporary_file_descriptor, char **environment_variables);

#endif
