# Pipe Up

pipe.c is a low-level implementation in C of the pipe (|) operator in Unix command line. 

## Building

Firstly, check if there is at least one command provided, else throw error
For every command between first to second last commands:
    Create a pipe between the parent (pipe) and a forked child process (for the current command)
    Within the forked child process:
        Map the write end of the pipe to stdout of the parent process
        Execute the current command in a forked child process; if failed, output the error to stderr and exit the parent process directly
    Parent process wait for the child process to finish
    Map read end of the pipe to stdin of the parent process, allowing next child process to read from output of the current child process
    Close both ends of the current pipe
Execute the last command in the parent process, which output to stdout; if failed, output the error to stderr and exit

## Running

Use "make" command to build the pipe executable file
Use "./pipe command_1 ... command_n" (commands seperated by space) to see the printed final output of all piped commands
e.g. "./pipe ls cat wc" is same as "ls | cat | wc" in command line

## Cleaning up

Use "make clean" command to clean all binary files
