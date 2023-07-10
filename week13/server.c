#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 8080

void reverseString(char *str)
{
    int i, j;
    char temp;
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

int main()
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }

    // Set server information
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Failed to bind socket");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0)
    {
        perror("Failed to listen for connections");
        exit(1);
    }

    printf("Server listening on port %d\n", SERVER_PORT);

    while (1)
    {
        // Accept a client connection
        clientAddrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0)
        {
            perror("Failed to accept client connection");
            exit(1);
        }

        // Receive and reverse the string from the client
        while (1)
        {
            memset(buffer, 0, sizeof(buffer));
            if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0)
            {
                perror("Failed to receive message from client");
                exit(1);
            }

            printf("Received: %s\n", buffer);

            if (strncmp(buffer, "quit", 4) == 0)
                break;

            // Reverse the string
            reverseString(buffer);

            // Send the reversed string back to the client
            if (send(clientSocket, buffer, strlen(buffer), 0) < 0)
            {
                perror("Failed to send message to client");
                exit(1);
            }
        }

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
