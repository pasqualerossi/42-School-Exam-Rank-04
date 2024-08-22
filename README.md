# Exam Question

This exam has 1 question, microshell:

- [Microshell.c](https://github.com/pasqualerossi/42-School-Exam-Rank-04/blob/main/microshell.c)

if you can make this code shorter, but readable, let me know!

<br>

## Excepted Files

- microshell.c

## Subject Text

Allowed functions: 

> malloc, free, write, close, fork, waitpid, signal, kill, exit, chdir, execve, dup, dup2, pipe, strcmp, strncmp


## The Program
Write a program that will behave like executing a shell command

- The command line to execute will be the arguments of this program

- Executable's path will be absolute or relative but your program must not build a path (from the PATH variable for example)

- You must implement "|" and ";" like in bash
	- we will never try a "|" immediately followed or preceded by nothing or "|" or ";"

- Your program must implement the built-in command cd only with a path as argument (no '-' or without parameters)
	- if cd has the wrong number of argument your program should print in STDERR "error: cd: bad arguments" followed by a '\n'
	- if cd failed your program should print in STDERR "error: cd: cannot change directory to path_to_change" followed by a '\n' with path_to_change replaced by the argument to cd
	- a cd command will never be immediately followed or preceded by a "|"

- You don't need to manage any type of wildcards (*, ~ etc...)

- You don't need to manage environment variables ($BLA ...)

- If a system call, except execve and chdir, returns an error your program should immediatly print "error: fatal" in STDERR followed by a '\n' and the program should exit

- If execve failed you should print "error: cannot execute executable_that_failed" in STDERR followed by a '\n' with executable_that_failed replaced with the path of the failed executable (It should be the first argument of execve)

- Your program should be able to manage more than hundreds of "|" even if we limit the number of "open files" to less than 30.

## Example

for example this should work:
```
$>./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
microshell
i love my microshell
$>

>./microshell 
```

## Hints
- Don't forget to pass the environment variable to execve
- Do not leak file descriptors!

## Commented Code

```c
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

// Function to write an error message to stderr
int err(char *str)
{
    // Loop through each character in the string and write it to stderr
    while (*str)
        write(2, str++, 1);
}

// Function to change the current working directory
int cd(char **argv, int i)
{
    // If the number of arguments is not 2, print error and exit
    if (i != 2)
        return err("error: cd: bad arguments\n"), 1;
    // If changing the directory fails, print error and exit
    if (chdir(argv[1]) == -1)
        return err("error: cd: cannot change directory to "), err(argv[1]), err("\n"), 1;
    return 0;
}

// Function to set pipe
// end == 1 sets stdout to act as write end of our pipe
// end == 0 sets stdin to act as read end of our pipe
void set_pipe(int has_pipe, int *fd, int end)
{
	if (has_pipe && (dup2(fd[end], end) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		err("error: fatal\n"), exit(1);
}

// Function to execute a command
int exec(char **argv, int i, char **envp)
{
    int has_pipe, fd[2], pid, status;

    // Check if the command includes a pipe
    has_pipe = argv[i] && !strcmp(argv[i], "|");

    // If the command is 'cd', execute it
    if (!has_pipe && !strcmp(*argv, "cd"))
        return cd(argv, i);

    // If the command includes a pipe and creating the pipe fails, print error and exit
    if (has_pipe && pipe(fd) == -1)
        err("error: fatal\n"), exit(1);

    // Fork the process and if the fork fails, print error and exit
    if ((pid = fork()) == -1)
	err("error: fatal\n"), exit(1);
    if (!pid)
    {
        argv[i] = 0;
        // If the command includes a pipe, set write end of pipe, if it fail print error and exit
        set_pipe(has_pipe, fd, 1);
        // If the command is 'cd', execute it
        if (!strcmp(*argv, "cd"))
            exit(cd(argv, i));
        // Execute the command
        execve(*argv, argv, envp);
        // If executing the command fails, print error and exit
        err("error: cannot execute "), err(*argv), err("\n"), exit(1);
    }

    // Wait for the child process to finish
    waitpid(pid, &status, 0);
    // If the command includes a pipe, set write end of pipe, if it fail print error and exit
    set_pipe(has_pipe, fd, 0);
    // Return the exit status of the child process
    return WIFEXITED(status) && WEXITSTATUS(status);
}

int main(int, char **argv, char **envp)
{
    int    i = 0, status = 0;

    // Loop through each following argument
    while (argv[i])
    {
        // Move the pointer to the next argument after the last delimeter / first argument
    	argv += i + 1;
    	i = 0;
        // Loop through each argument until a pipe or semicolon is found
    	while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
        // If there are arguments, execute them
    	if (i)
			status = exec(argv, i, envp);
    }
    return status;
}
```

## Exam Practice Tool

Practice the exam just like you would in the real exam - https://github.com/JCluzet/42_EXAM
