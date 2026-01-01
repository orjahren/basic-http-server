// src/main.c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO: Burde ha et opplegg for dynamisk portkonfigurasjon
#define PORT 8080

void writePortFile(const char *filename, int port)
{

    printf("Writing port %d to file %s\n", port, filename);
    FILE *file = fopen(filename, "w");
    if (file != NULL)
    {
        fprintf(file, "%d\n", port);
        fclose(file);
    }
    else
    {
        printf("Failed to open file %s for writing\n", filename);
    }
}

// TODO: Implement a basic HTTP server that listens on PORT and responds with
// "Hello, World!" to any GET request.

int startHttpServer(int port)
{
    printf("Starting HTTP server on port %d...\n", port);
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    // if (setsockopt(server_fd, SOL_SOCKET,
    // SO_REUSEADDR | SO_REUSEPORT, &opt,
    // sizeof(opt)))
    // Source - https://stackoverflow.com/a
    // Posted by AKHIL
    // Retrieved 2026-01-01, License - CC BY-SA 4.0

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))

    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    while (1)
    {

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 &addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // subtract 1 for the null
        // terminator at the end
        valread = read(new_socket, buffer,
                       1024 - 1);
        printf("%s\n", buffer);
        printf("Size read: %zd\n", valread);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");

        // closing the connected socket
        close(new_socket);
    }

    // closing the listening socket
    close(server_fd);
    return 0;
}

int main(void)
{
    printf("Hello, basic HTTP server!\n");

    writePortFile("server.port", PORT);

    startHttpServer(PORT);

    return 0;
}
