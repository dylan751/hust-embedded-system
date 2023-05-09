#include <stdio.h>
#include <stdlib.h>

int main( )
{
  if(system("ls -l")==-1){
    perror("system");
    return -1;
  }

  return 0;
}
