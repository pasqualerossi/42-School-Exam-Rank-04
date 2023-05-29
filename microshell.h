#ifndef MICROSHELL_H
# define MICROSHELL_H

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int write_error(char *string, char *argv);
int ft_exe(char **argv, int i, int temporary_file_descriptor, char **environment_variables);

#endif
