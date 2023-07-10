#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

#define Request 1
#define MAX_RECORD 10
#define NUMBER_OF_PROCESS 3
#define FINISH 4

struct msg_buffer
{
    long mtype;
    int data;
    int processId;
};

int main()
{
    key_t key = ftok(".", '1');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    int nextProcess = 1; // Next process that can access the memory
    struct msg_buffer inquiryRecord[MAX_RECORD];
    int recordSize = 0;

    while (1)
    {
        printf("Next process: %d\n", nextProcess);
        // Wait for memory access inquiry from memAccProc
        struct msg_buffer msg;
        if (msgrcv(msgid, &msg, sizeof(msg.data), Request, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }

        if (msg.data != nextProcess)
        {
            // Not the next process, store the inquiry in the record
            printf("Received inquiry from process %d with pid %d. Waiting for process %d to finish...\n", msg.data, msg.processId, nextProcess);
            inquiryRecord[recordSize] = msg;
            recordSize++;
        }
        else
        {
            // The next process, send signal to the process
            printf("Received inquiry from process %d with pid %d. Allowing access to memory...\n", msg.data, msg.processId);
            kill(msg.processId, SIGUSR1);

            // Wait for processing finished message from the process
            if (msgrcv(msgid, &msg, sizeof(msg.data), FINISH, 0) == -1)
            {
                perror("msgrcv");
                exit(1);
            }
            printf("Received finished message from process %d", msg.data);
            nextProcess = nextProcess % 3 + 1;
            if (recordSize > 0)
            {
                while (nextProcess == inquiryRecord[0].data)
                {
                    kill(inquiryRecord[0].processId, SIGUSR1);
                    if (msgrcv(msgid, &msg, sizeof(msg.data), FINISH, 0) == -1)
                    {
                        perror("msgrcv");
                        exit(1);
                    }
                    for (int i = 0; i < recordSize; i++)
                    {
                        inquiryRecord[i] = inquiryRecord[i + 1];
                    }
                    recordSize--;
                    nextProcess = nextProcess % 3 + 1;
                }
            }
        }
    }

    return 0;
}
