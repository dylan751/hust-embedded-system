#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

char *str;

void sig_handler(int signum)
{
    if (signum == SIGALRM)
    {
        printf("Alarm signal. str = %s\n", str);
        exit(0);
    }
    if (signum == SIGINT)
    {
        printf("Interrupt signal. str = %s\n", str);
        exit(0);
    }
}

int main(int argc, char const *argv[])
{

    // arg 1: number of seconds
    // arg 2: string

    int n = (int)strtol(argv[1], NULL, 10);
    str = strdup(argv[2]);

    signal(SIGALRM, sig_handler);
    signal(SIGINT, sig_handler);
    alarm(n);

    for (int i = 1;; i++)
    {
        printf("i = %d\n", i);
        sleep(1);
    }
}