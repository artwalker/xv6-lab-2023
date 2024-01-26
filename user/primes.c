#include "kernel/types.h"
//#include "kernel/stat.h"
#include "user/user.h"

#define READEND 0
#define WRITEEND 1
#define MAX_NUM 35

void primeSieve(int pfd[]);

int main(void)
{
    int pfd[2]; // pipe file descriptors
    pipe(pfd);

    if (fork() == 0)
    {
        close(pfd[WRITEEND]);
        primeSieve(pfd);
        exit(0);
    }
    else
    {
        close(pfd[READEND]);
        for (int i = 2; i <= MAX_NUM; i++)
        {
            write(pfd[WRITEEND], &i, sizeof(i));
        }
        close(pfd[WRITEEND]);
        wait(0);
    }
    exit(0);
}

void primeSieve(int pfd[])
{
    int prime;
    if (read(pfd[READEND], &prime, sizeof(prime)) == 0)
    {
        close(pfd[READEND]);
        return;
    }

    // Necessary to ensure that the first number printed is 2
    fprintf(1, "prime %d\n", prime);

    int npfd[2]; // new pipe file descriptors
    pipe(npfd);
    if (fork() == 0)
    {
        close(npfd[WRITEEND]);
        close(pfd[READEND]);
        primeSieve(npfd);
        exit(0);
    }
    else
    {
        close(npfd[READEND]);
        int num;
        while (read(pfd[READEND], &num, sizeof(num)) > 0)
        {
            if (num % prime != 0)
            {
                write(npfd[WRITEEND], &num, sizeof(num));
            }
        }
        close(pfd[READEND]);
        close(npfd[WRITEEND]);
        wait(0);
    }
}
