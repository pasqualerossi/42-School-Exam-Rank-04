# Exam Question

This exam has 1 question, microshell:

- [Microshell.c](https://github.com/pasqualerossi/42-School-Exam-Rank-04/blob/main/microshell.c)
- [Microshell.h](https://github.com/pasqualerossi/42-School-Exam-Rank-04/blob/main/microshell.h)

if you can make this code shorter, but readable, let me know!

<br>

## Excepted Files

- microshell.c

- microshell.h

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
```

## Hints
- Don't forget to pass the environment variable to execve
- Do not leak file descriptors!

## Exam Practice Tool

Practice the exam just like you would in the real exam - https://github.com/JCluzet/42_EXAM

# Code Commented
```c
int err(char *str)
{
    while (*str)
        write(2, str++, 1); // Writes each character of 'str' to standard error (file descriptor 2)
    return 1;
}

int cd(char **argv, int i)
{
    if (i != 2)
        return err("error: cd: bad arguments\n"); // Calls 'err' function and returns 1 if the number of arguments is not 2
    else if (chdir(argv[1]) == -1)
        return err("error: cd: cannot change directory to "), err(argv[1]), err("\n"); // Calls 'err' function with error messages if 'chdir' fails
    return 0;
}

int exec(char **argv, char **envp, int i)
{
    int fd[2];
    int status;
    int has_pipe = argv[i] && !strcmp(argv[i], "|"); // Checks if there is a pipe symbol ('|') present in the argument array

    if (has_pipe && pipe(fd) == -1)
        return err("error: fatal\n"); // Calls 'err' function and returns 1 if pipe creation fails

    int pid = fork(); // Creates a child process
    if (!pid)
    {
        argv[i] = 0;
        if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
            return err("error: fatal\n"); // Calls 'err' function and returns 1 if setting up pipe I/O fails
        execve(*argv, argv, envp); // Executes the command specified by 'argv'
        return err("error: cannot execute "), err(*argv), err("\n"); // Calls 'err' function if execution fails
    }

    waitpid(pid, &status, 0); // Waits for the child process to complete
    if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
        return err("error: fatal\n"); // Calls 'err' function and returns 1 if restoring standard I/O fails
    return WEXITSTATUS(status); // Extracts the exit status of the child process
}

int main(int argc, char **argv, char **envp)
{
    int i = 0;
    int status = 0;

    if (argc > 1)
    {
        while (argv[i] && argv[++i])
        {
            argv += i;
            i = 0;
            while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
                i++; // Counts the number of arguments until a pipe ('|') or semicolon (';') is encountered
            if (!strcmp(*argv, "cd"))
                status = cd(argv, i); // Calls 'cd' function if the first argument is "cd"
            else if (i)
                status = exec(argv, envp, i); // Calls 'exec' function for executing a command
        }
    }
    return status; // Returns the status of the last command executed
}
```
