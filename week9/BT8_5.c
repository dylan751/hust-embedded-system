#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main()
{
  pthread_mutex_t pm;
  pthread_mutexattr_t pmattr;

  // Initialize the mutex attribute object
  pthread_mutexattr_init(&pmattr);

  // Set mutex attribute(Setting for using mutex to inter process)
  if (pthread_mutexattr_setpshared(&pmattr, PTHREAD_PROCESS_SHARED) != 0) {
    perror("pthread_mutexattr_setpshared");
    exit(1);
  }
 // Initialize mutex
  pthread_mutex_init(&pm, &pmattr);

  // Lock mutex
  if (pthread_mutex_lock(&pm) != 0) {
    perror("pthread_mutex_lock");
    exit(1);
  }

printf("mutex lock\n");

  // Unlock mutex
  if (pthread_mutex_unlock(&pm) != 0) {
    perror("pthread_mutex_unlock");
    exit(1);
  }
  printf("mutex unlock\n");

  return 0;
}

