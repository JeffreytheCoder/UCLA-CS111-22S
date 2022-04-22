#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // if no command provided, output the error to stderr and exit
    if (argc < 2)
    {
        fprintf(stderr, "Must have at least one argument after command!");
        exit(EXIT_FAILURE);
    }

    // for 1st to 2nd last commands
    int i;
    for (i = 1; i < argc - 1; i++)
    {
        // create pipe between parent and child process
        int pipefd[2];

        // if create pipe failed, output the error to stderr and exit
        if (pipe(pipefd) != 0)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        // run the process in a forked child process, not the parent
        if (fork() == 0)
        {
            // map the write end of the pipe to stdout of the parent process
            dup2(pipefd[1], 1);

            // execute the command, which output is piped to the read end of the pipe
            execlp(argv[i], argv[i], NULL);

            // if execlp failed, output the error to stderr and exit the parent process directly
            perror("execlp");
            exit(EXIT_FAILURE);

            // if execlp succeeded, the parent process will wait for the child process to finish
            wait(NULL);
        }

        // map the read end of the pipe to stdin of the parent process
        // allows next child process to read from output of the current child process
        dup2(pipefd[0], 0);

        // close the current pipe
        close(pipefd[0]);
        close(pipefd[1]);
    }

    // execute the last command in the parent process, which output to stdout
    execlp(argv[i], argv[i], NULL);

    // if execlp failed, output the error to stderr and exit
    perror("execlp");
    exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
}