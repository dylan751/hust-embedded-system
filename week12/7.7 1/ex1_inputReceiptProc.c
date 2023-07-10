#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_TYPE 1

struct msg_buffer
{
    long mtype;
    int data;
};

int main()
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
        {
            // Child process
            close(pipefd[1]); // Close the write end of the pipe
            key_t key = ftok(".", 'a');
            int msgid = msgget(key, 0666 | IPC_CREAT);
            if (msgid == -1)
            {
                perror("msgget");
                exit(1);
            }

            while (1)
            {
                int input_value;
                read(pipefd[0], &input_value, sizeof(input_value)); // Receive input from parent process

                // Send the received input value to program2 by message queue
                struct msg_buffer msg;
                msg.mtype = MSG_TYPE;
                msg.data = input_value;

                if (msgsnd(msgid, &msg, sizeof(msg.data), 0) == -1)
                {
                    perror("msgsnd");
                    exit(1);
                }

                // Wait for message from program2
                msgrcv(msgid, &msg, sizeof(msg.data), MSG_TYPE + 1, 0);
                printf("\nChild process: Received calculation result: %d\n", msg.data);
            }

            close(pipefd[0]); // Close the read end of the pipe

            exit(0);
        }
    else
    {
        // Parent process
        close(pipefd[0]); // Close the read end of the pipe

        while (1)
        {
            int input_value;
            printf("Enter an integer value (or -1 to exit): ");
            scanf("%d", &input_value);

            if (input_value == -1)
            {
                break; // Exit the loop and terminate the program
            }

            write(pipefd[1], &input_value, sizeof(input_value)); // Send input to child process
        }

        // Close the write end of the pipe
        close(pipefd[1]);

        exit(0);
    }

    return 0;
}
