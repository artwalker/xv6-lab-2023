#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    int p2c[2], c2p[2];
    char buf = 'x';

    pipe(p2c);
    pipe(c2p);

    if (fork() == 0)
    {
        // Child process
        close(p2c[1]);
        close(c2p[0]);

        read(p2c[0], &buf, 1);
        printf("%d: received ping\n", getpid());

        write(c2p[1], &buf, 1);
        exit(0);
    }
    else
    {
        // Parent process
        close(p2c[0]);
        close(c2p[1]);

        write(p2c[1], &buf, 1);
        read(c2p[0], &buf, 1);

        printf("%d: received pong\n", getpid());
        exit(0);
    }
}