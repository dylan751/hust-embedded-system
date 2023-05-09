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
  int shmsize;
  int shmid;
  key_t keyval;
  int *ptr;
  int *head;

  // keyval = ftok(KEYFILE_PATH, (int)PROJ_CHAR);
  keyval = ftok(KEYFILE_PATH, PROJ_ID);

  shmsize = SHMSIZE;

  // Get the shared memory ID
  if ((shmid = shmget(keyval, shmsize * sizeof(int),
                      IPC_CREAT | 0660)) == -1)
  {
    perror("shmget");
    exit(1);
  }

  // Attach the shared memory
  if ((head = ptr = (int *)shmat(shmid, 0, 0)) == (int *)-1)
  {
    perror("shmat");
    exit(1);
  }

  // Write to the shared memory
  for (i = 0; i != shmsize; i++)
  {
    *ptr++ = i;
  }

  // Detach the shared memory
  if (shmdt(head) == -1)
  {
    perror("shmdt");
    exit(1);
  }

  return 0;
}
