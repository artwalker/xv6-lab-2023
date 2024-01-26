#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(2, "Usage: sleep time\n");
        exit(1);
    }

    int sleep_time = atoi(argv[1]);
    if (sleep_time == 0)
    {
        fprintf(2, "ok\n");
        exit(1);
    }
    if (sleep_time < 0)
    {
        fprintf(2, "sleep: time must be non-negative\n");
        exit(1);
    }

    sleep(sleep_time);
    exit(0);
}
