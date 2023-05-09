#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define PROC_OK  0
#define PROC_NG  -1

/**
 * The main processing
 */
int main(void)
{
  printf("main           : start\n");

  char *args[] = {"ls", "-l", NULL};
  int iRet = 0;
  pid_t pid = 0;
  int  status = 0;

  // Creating child process
  switch (pid = fork()){
    case -1:
      // Process creating error processing
      perror("processGenerate fork");
      exit(1);
    case 0:
      // Child process processing
      iRet = execv("/bin/ls", args);

      // It doesn't pass when execv succeeds. 
      printf("execv ret = %d\n", iRet);
      if(iRet == PROC_NG)
      {
        exit(1);
      }

    default:
      // Parent process processing
      printf("Parent Process ChildProcPid = %d\n", pid);

      // Wait for the state transition of the child process. 
      pid=wait(&status);
      printf("pid=%d,status=%d\n", pid, status);
      break;
  }

  printf("main           : end\n");
  return 0;
}