#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

int main()
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }

    // Set server information
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Failed to connect to the server");
        exit(1);
    }

    // Receive and display the reversed string from the server
    while (1)
    {
        // Send the string "Abcdef" to the server
        char buffer[MAX_BUFFER_SIZE];
        printf("Input string: ");
        scanf("%s", buffer);
        if (send(clientSocket, buffer, strlen(buffer), 0) < 0)
        {
            perror("Failed to send message to the server");
            exit(1);
        }
        
        if (strcmp(buffer, "quit") == 0)
            break;

        memset(buffer, 0, sizeof(buffer));
        if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0)
        {
            perror("Failed to receive message from the server");
            exit(1);
        }

        printf("Received: %s\n", buffer);
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
