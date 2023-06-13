#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 100

struct message
{
    long mtype;
    char mtext[MAX_MESSAGE_SIZE];
};

int main()
{
    key_t key;
    int msgid;
    struct message msg;

    // Generate a unique key for the message queue
    key = ftok(".", 'A');
    if (key == -1)
    {
        perror("ftok");
        exit(1);
    }

    // Create a message queue with read/write permissions
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    // Receive messages from the message queue
    printf("Waiting for messages...\n");
    while (1)
    {
        // Receive the message of type 1
        if (msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), 1, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }

        printf("Received message: %s\n", msg.mtext);

        // Check if the received message is an exit command
        if (strcmp(msg.mtext, "exit") == 0)
        {
            break;
        }
    }

    // Remove the message queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(1);
    }

    return 0;
}
