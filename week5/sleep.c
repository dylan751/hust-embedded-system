#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

float time_diff(struct timeval *start, struct timeval *end)
{
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}

void call_alarm(int sig)
{
    printf("ALARM CALLED\n");
}


int main() {
    struct timeval start;
    struct timeval end;

    unsigned int seconds = 10;
    unsigned int usecs = 5;

    signal(SIGALRM, call_alarm);
// --------------------------------
    gettimeofday(&start, NULL);
    printf("start: %.8ld:%.8ld\n", start.tv_sec, start.tv_usec);
    sleep(seconds);
    alarm(1);
    gettimeofday(&end, NULL);
    printf("end: %.8ld:%.8ld\n", end.tv_sec, end.tv_usec);

    printf("sleep(%d)  time spent: %0.8f sec\n", seconds, time_diff(&start, &end));

// --------------------------------
    signal(SIGALRM, call_alarm);

    gettimeofday(&start, NULL);
    printf("start: %.8ld:%.8ld\n", start.tv_sec, start.tv_usec);
    usleep(usecs);
    alarm(1);
    gettimeofday(&end, NULL);
    printf("end: %.8ld:%.8ld\n", end.tv_sec, end.tv_usec);

    printf("unsleep(%d) time spent: %0.8f sec\n", usecs, time_diff(&start, &end));
// --------------------------------
    signal(SIGALRM, call_alarm);

    struct timespec remaining, request = { 5, 10 };
    gettimeofday(&start, NULL);
    printf("start: %.8ld:%.8ld\n", start.tv_sec, start.tv_usec);
    int response = nanosleep(&request, &remaining);
    alarm(1);
    gettimeofday(&end, NULL);
    printf("end: %.8ld:%.8ld\n", end.tv_sec, end.tv_usec);

    if (response == 0) {
        printf("Nap was Successful.\n");
    }
    else {
        printf("Nap was Interrupted.\n");
    }

    printf("nanosleep(5,10) time spent: %0.8lf sec\n", time_diff(&start, &end));
    exit(EXIT_SUCCESS);
}