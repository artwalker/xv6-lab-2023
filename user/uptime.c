#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int uptime_ticks = uptime();
    printf("Uptime in ticks: %d\n", uptime_ticks);
    exit(0);
}
