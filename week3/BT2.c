#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define SIG_OK 0
#define SIG_NG -1
#define LOOP_ON   0
#define LOOP_OFF  1

// Global variable
int loopFlag = LOOP_ON;  /* Loop flag */

// Prototype declaration
void sigHandleSighup(int sigNo);

/**
 * The main processing
 * 
 * param  none
 * 
 * return SIG_OK fix
 */
int main()
{
  printf("main       : start\n");
// Signal handler function registration
  signal(SIGHUP, sigHandleSighup);

  // Loop during the time the loop flag is ON
  while(loopFlag == LOOP_ON){
    printf("Looping\n");
  }

  printf("main       : end\n");
  return SIG_OK;
}

/**
 * Signal handler function that receives signal SIGHUP
 * 
 * param sigNo Received signal number
 *
 * return  none
 */
void sigHandleSighup(int sigNo)
{
  printf("Signal No. %d\n", sigNo);
  loopFlag = LOOP_OFF;
  return;
}
