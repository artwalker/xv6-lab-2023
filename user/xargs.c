#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

void 
xargs(int n, int argc, char *argv[])
{
    int i;
    char c, *line;
    char *args[MAXARG];
    int line_len = 0;
    int line_capacity = 512;

    // initialize args with the command and its arguments
    for (i = 0; i < argc; i++)
    {
        args[i] = argv[i];
    }

    // allocate initial memory for line
    line = malloc(line_capacity);
    if (line == 0)
    {
        fprintf(2, "malloc failed\n");
        exit(1);
    }

    // read from standard input character by character
    while (read(0, &c, 1) > 0)
    {
        if (c == '\n' || c == '\0')
        {
            if (line_len > 0)
            {
                line[line_len] = '\0'; // null-terminate the line

                // Check if args array can accept more arguments
                if (argc >= MAXARG - 2)
                {
                    fprintf(2, "argument list too long\n");
                    exit(1);
                }

                // add the line of input as an argument
                args[argc] = line;
                args[argc + 1] = 0;

                // fork and execute the command
                if (fork() == 0)
                {
                    exec(args[0], args);
                    exit(0); // exit the child process
                }
                else
                {
                    wait(0);
                }

                // reset for the next line
                line_len = 0;
            }
        }
        else
        {
            line[line_len++] = c;

            // Check for line buffer overflow
            if (line_len >= line_capacity - 1)
            {
                fprintf(2, "line buffer exceeded\n");
                exit(1);
            }
        }
    }

    free(line); // free the line buffer
}

int 
main(int argc, char *argv[])
{
    int n = 1; // default value for -n option
    int i;

    // parse command line options
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc)
        {
            n = atoi(argv[i + 1]);
            i++; // skip next argument
        }
        else
        {
            break; // stop parsing options
        }
    }

    // pass the value of the -n option to the xargs function
    xargs(n, argc - i, argv + i);

    exit(0);
}
