// Implementation of accOrderCtrl.c:

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>

#define MSG_KEY 9876

#define MAX_QUEUE_SIZE 10

typedef struct {
    long mtype;
    int process_num;
    int process_id;
} Message;

int main() {
    int msqid = msgget(MSG_KEY, IPC_CREAT | 0666);

    int next_access = 1;
    Message inquiry_record[MAX_QUEUE_SIZE];
    int inquiry_record_size = 0;

    while (1) {
        Message msg;
        msgrcv(msqid, &msg, sizeof(msg), 0, 0);

        // Check if inquiry msg or finished msg
        if (msg.mtype == 1) {
            printf("Process %d inquired\n", msg.process_num);
            // Check if inquiry_record is empty and msg.process_num equals next_access 
            if (msg.process_num == next_access) {
                // Send signal to process
                printf("Process %d is next\n", msg.process_num);
                printf("Sending signal to process %d\n", msg.process_id);
                kill(msg.process_id, SIGUSR1);
            } else {
                // Add process to inquiry_record
                printf("Process %d is inquired\n", msg.process_num);
                inquiry_record[inquiry_record_size] = msg;
                inquiry_record_size++;
            }
        }

        if (msg.mtype == 2) {
            // Update next_access and check if next_access is in inquiry_record
            printf("Process %d finished\n", msg.process_num);
            next_access = next_access % 3 + 1;
            int next_process_id = -1;
            for (int i = 0; i < inquiry_record_size; i++) {
                if (inquiry_record[i].process_num == next_access) {
                    next_process_id = inquiry_record[i].process_id;
                    // Remove process from inquiry_record
                    for (int j = i; j < inquiry_record_size - 1; j++) {
                        inquiry_record[j] = inquiry_record[j + 1];
                    }
                    inquiry_record_size--;
                    break;
                }
            }
            if (next_process_id == -1) {
                printf("No process inquired\n");
                continue;
            }
            // Send signal to process
            printf("Process %d is next\n", next_access);
            printf("Sending signal to process %d\n", next_access);
            kill(next_process_id, SIGUSR1);
        }
    }

    return 0;
}