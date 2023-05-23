// C program to implement one side of FIFO
// This side reads first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd1;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(myfifo, 0666);

    char str1[256], str2[256];

    // First open in read only and read
    fd1 = open(myfifo,O_RDONLY);
    read(fd1, str1, 80);

    // Print the read string and close
    printf("Data:\n%s\n", str1);
    close(fd1);
    unlink(myfifo);
    return 0;
}