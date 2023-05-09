#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZE 10
#define KEYFILE_PATH "keyfilepath"
#define PROJ_ID 1

int main()
{
    int i;
    int shmid;
    key_t keyval;
    int *ptr, *temp_ptr;

    keyval = ftok(KEYFILE_PATH, PROJ_ID);
    if ((shmid = shmget(keyval, SHMSIZE * sizeof(int), 0660)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    if ((ptr = (int *)shmat(shmid, 0, 0)) == (int *)-1)
    {
        perror("shmat");
        exit(1);
    }

    temp_ptr = ptr;
    for (i = 0; i < SHMSIZE; i++)
    {
        printf("Shared Memory[%d] = %d\n", i, *temp_ptr++);
    }

    if (shmdt((void *)ptr) == -1)
    {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
