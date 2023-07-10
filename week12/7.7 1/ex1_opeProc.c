#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <pthread.h>

#define MSG_TYPE 1
#define SHM_KEY 12345

struct msg_buffer
{
    long mtype;
    int data;
};

int main()
{
    key_t key = ftok(".", 'a');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    int shmid = shmget(SHM_KEY, sizeof(int), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("shmget");
        exit(1);
    }

    int *shmemVal = (int *)shmat(shmid, NULL, 0);
    if (shmemVal == (int *)(-1))
    {
        perror("shmat");
        exit(1);
    }

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    while (1)
    {
        // Receive message from program1
        struct msg_buffer msg;
        msgrcv(msgid, &msg, sizeof(msg.data), MSG_TYPE, 0);

        // Lock the mutex before accessing shared memory
        pthread_mutex_lock(&mutex);

        // Read the value from shared memory
        int shared_value = *shmemVal;

        // Sleep for 5 seconds
        sleep(5);

        // Perform the calculation
        int result = msg.data + shared_value;

        // Write the result to shared memory
        *shmemVal = result;

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Send the result back to program1
        msg.mtype = MSG_TYPE + 1;
        msg.data = result;

        if (msgsnd(msgid, &msg, sizeof(msg.data), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
    }

    // Detach shared memory
    shmdt(shmemVal);

    // Remove shared memory
    shmctl(shmid, IPC_RMID, NULL);

    // Remove the message queue
    msgctl(msgid, IPC_RMID, NULL);

    exit(0);
}
