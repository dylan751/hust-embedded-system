// Deletes message queue in C.
// For educational purposes only.
// Author: Vaclav Bohac

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(void)
{
    int mask, msgid;

    key_t key = ftok(".", 'a');
    mask = 0666;
    msgid = msgget(key, mask);

    if (msgid == -1)
    {
        printf("Message queue does not exists.\n");
        exit(EXIT_SUCCESS);
    }

    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        fprintf(stderr, "Message queue could not be deleted.\n");
        exit(EXIT_FAILURE);
    }

    printf("Message queue was deleted.\n");

    return EXIT_SUCCESS;
}