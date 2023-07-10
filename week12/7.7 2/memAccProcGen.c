#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

#define REQUEST 1
#define SHMEM_KEY 1234
#define SEM_KEY 5678
#define FINISH 4

struct msg_buffer
{
    long mtype;
    int data;
    int processId;
};

int shmid; // Shared memory ID
int semid; // Semaphore ID

void handleSignal(int signal)
{
    // Remove the shared memory and semaphore
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(1);
    }

    if (semctl(semid, 0, IPC_RMID) == -1)
    {
        perror("semctl");
        exit(1);
    }

    printf("\nShared memory and semaphore removed. Exiting...\n");

    exit(0);
}

int main()
{
    signal(SIGINT, handleSignal); // Register signal handler for Ctrl+C

    srand(time(NULL)); // Initialize random seed
    // Create shared memory
    shmid = shmget(SHMEM_KEY, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(1);
    }

    // Create semaphore
    semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semid == -1)
    {
        perror("semget");
        exit(1);
    }

    // Initialize semaphore value to 1
    union semun
    {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1)
    {
        perror("semctl");
        exit(1);
    }

    int i;
    for (i = 1; i <= 3; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(1);
        }

        if (pid == 0)
        {
            // Child process (memAccProc1, memAccProc2, memAccProc3)
            key_t key = ftok(".", '1');
            int msgid = msgget(key, 0666 | IPC_CREAT);
            if (msgid == -1)
            {
                perror("msgget");
                exit(1);
            }

            while (1)
            {
                // Sleep for a random time between 1 to 5 seconds
                int sleep_time = rand() % 5 + 1;
                printf("Process %d is sleeping for %d seconds", i, sleep_time);
                sleep(sleep_time);

                // Send memory access inquiry to accOrderCtrl
                struct msg_buffer msg;
                msg.mtype = REQUEST;
                msg.data = i;
                msg.processId = getpid();

                if (msgsnd(msgid, &msg, sizeof(msg.data), 0) == -1)
                {
                    perror("msgsnd");
                    exit(1);
                }

                // Wait for SIGUSR1 signal
                pause();

                // Lock the semaphore
                struct sembuf sops;
                sops.sem_num = 0;
                sops.sem_op = -1;
                sops.sem_flg = 0;
                if (semop(semid, &sops, 1) == -1)
                {
                    perror("semop");
                    exit(1);
                }

                // Write to shared memory
                int *shmemVal = (int *)shmat(shmid, NULL, 0);
                if (shmemVal == (int *)-1)
                {
                    perror("shmat");
                    exit(1);
                }

                if (i == 1)
                {
                    // memAccProc1: Write "X=10, Y=0" to shmemVal
                    printf("Process 1: Write X=10, Y=0 to shmemVal\n");
                    shmemVal[0] = 10;
                    shmemVal[1] = 0;
                }
                else if (i == 2)
                {
                    // memAccProc2: Calculate (100 / X) and write "X=0, Y=10" to shmemVal
                    int X = shmemVal[0];
                    printf("Process 2: 100/X = %d", 100 / X);
                    shmemVal[0] = 0;
                    shmemVal[1] = 10;
                }
                else if (i == 3)
                {
                    // memAccProc3: Calculate (10000 / Y) and write "X=0, Y=0" to shmemVal
                    int Y = shmemVal[1];
                    printf("Process 3: 10000/Y = %d", 10000 / Y);
                    shmemVal[0] = 0;
                    shmemVal[1] = 0;
                }

                // Unlock the semaphore
                sops.sem_op = 1;
                if (semop(semid, &sops, 1) == -1)
                {
                    perror("semop");
                    exit(1);
                }

                // Send processing finished message to accOrderCtrl
                msg.mtype = FINISH;

                if (msgsnd(msgid, &msg, sizeof(msg.data), 0) == -1)
                {
                    perror("msgsnd");
                    exit(1);
                }
            }

            exit(0);
        }
    }
    while (1)
    {
        // Do nothing
    }

    return 0;
}
