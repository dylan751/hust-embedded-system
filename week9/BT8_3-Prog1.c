#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    char arr1[1024], dataBuff[1024];
    // Open FIFO for write only
    fd = open(myfifo, O_WRONLY);

    //read dataFile
    FILE *p = fopen("DataFile.txt", "r");
    fgets(dataBuff, 1024, p);
    // Write the input dataBuffing on FIFO
    // and close it
    write(fd, dataBuff, strlen(dataBuff)+1);
    close(fd);
    
    return 0;
}