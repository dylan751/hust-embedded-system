// Implementation of memAccProcGen.c:

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define SHM_KEY 1234
#define SEM_KEY 5678
#define MSG_KEY 9876

#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2

#define MAX_QUEUE_SIZE 10

typedef struct {
    long mtype;
    int process_num;
    int process_id;
} Message;

typedef struct {
    int x;
    int y;
} SharedMemory;

int sem_id, shm_id, msg_id;
SharedMemory *shmemVal;

volatile sig_atomic_t signal_received = 0;

void sigint_handler(int signal)
{
    // Remove the shared memory and semaphore
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(1);
    }

    if (semctl(sem_id, 0, IPC_RMID) == -1)
    {
        perror("semctl");
        exit(1);
    }

    printf("\nShared memory and semaphore removed. Exiting...\n");

    exit(0);
}

void sigusr1_handler(int signal) {
    printf("Received signal from accOrderCtrl\n");
    signal_received = 1;
}

void acquire_semaphore(int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

void release_semaphore(int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

void process1() {
    // Write "X=10, Y=0" to shared memory
    shmemVal->x = 10;
    shmemVal->y = 0;
    printf("Process 1: Wrote X=10, Y=0\n");
}

void process2() {
    // Calculate (100 / X) and write "X=0, Y=10" to shmemVal
    int x = shmemVal->x;
    printf("Process 2: Calculating 100 / %d = %d\n", x, 100 / x);
    shmemVal->x = 0;
    shmemVal->y = 10;
    printf("Process 2: Wrote X=0, Y=10\n");
}

void process3() {
    // Calculate (10000 / Y) and write "X=0, Y=0" to shmemVal
    int y = shmemVal->y;
    printf("Process 3: Calculating 10000 / %d = %d\n", y, 10000 / y);
    shmemVal->x = 0;
    shmemVal->y = 0;
    printf("Process 3: Wrote X=0, Y=0\n");
}

void process(int process_num) {
    Message msg;
    msg.mtype = 1;
    msg.process_num = process_num;
    msg.process_id = getpid();
    msgsnd(msg_id, &msg, sizeof(msg), 0);
    printf("Process %d: Sent inquiry\n", process_num);

    // Wait for signal from accOrderCtrl
    while (!signal_received) {
        pause();
    }

    // Reset the signal flag
    signal_received = 0;
    
    printf("Process %d: Received signal\n", process_num);

    // Acquire semaphore
    acquire_semaphore(SEM_MUTEX);

    switch (process_num) {
        case 1:
            process1();
            break;
        case 2:
            process2();
            break;
        case 3:
            process3();
            break;
    }

    // Release semaphore
    release_semaphore(SEM_MUTEX);

    msg.mtype = 2;
    msg.process_num = process_num;
    msg.process_id = getpid();
    msgsnd(msg_id, &msg, sizeof(msg), 0);
    printf("Process %d: Sent finished message\n", process_num);
}

int main() {
    signal(SIGINT, sigint_handler);
    signal(SIGUSR1, sigusr1_handler);
    // Set up semaphores
    sem_id = semget(SEM_KEY, 3, IPC_CREAT | 0666);
    semctl(sem_id, SEM_MUTEX, SETVAL, 1);
    semctl(sem_id, SEM_EMPTY, SETVAL, MAX_QUEUE_SIZE);
    semctl(sem_id, SEM_FULL, SETVAL, 0);

    // Set up shared memory
    shm_id = shmget(SHM_KEY, sizeof(SharedMemory), IPC_CREAT | 0666);
    shmemVal = (SharedMemory *) shmat(shm_id, NULL, 0);

    // Set up message queue
    msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);

    // Set up random number generator
    srand(time(NULL));

    // Create processes using fork()
    // process1 repeats for random interval (1~5 seconds)
    // process2 repeats for 1 second
    // process3 repeats for 5 seconds

    int pid = fork();
    if (pid == 0) {
        while (1) {
            sleep(rand() % 5 + 1);
            printf("Process 1: Woke up\n");
            process(1);
        }
    } else {
        pid = fork();
        if (pid == 0) {
            while (1) {
                sleep(1);
                printf("Process 2: Woke up\n");
                process(2);
            }
        } else {
            pid = fork();
            if (pid == 0) {
                while (1) {
                    sleep(5);
                    printf("Process 3: Woke up\n");
                    process(3);
                }
            }
        }
    }

    // Wait for all processes to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Detach shared memory
    shmdt(shmemVal);

    // Remove shared memory
    shmctl(shm_id, IPC_RMID, NULL);

    // Remove semaphores
    semctl(sem_id, 0, IPC_RMID);

    // Remove message queue
    msgctl(msg_id, IPC_RMID, NULL);

    return 0;
}
