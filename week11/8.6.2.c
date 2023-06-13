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

    // Generate the same key as Program 1
    key = ftok(".", 'A');
    if (key == -1)
    {
        perror("ftok");
        exit(1);
    }

    // Connect to the existing message queue
    msgid = msgget(key, 0666);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    // Get an arbitrary character string from the user
    printf("Enter a message: ");
    fgets(msg.mtext, MAX_MESSAGE_SIZE, stdin);

    // Remove the trailing newline character
    msg.mtext[strcspn(msg.mtext, "\n")] = '\0';

    // Set the message type to 1
    msg.mtype = 1;

    // Send the message to Program 1
    if (msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }

    printf("Message sent.\n");

    return 0;
}
