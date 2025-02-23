#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


// maximum application buffer
#define APP_MAX_BUFFER 1024
#define PORT 8080

int main() {
    // define the server and client file descriptors
    int server_fd, client_fd;

    // define client addresses
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // define the application buffer where we receive the requests
    // data will be moved from receive buffer to here
    char buffer[APP_MAX_BUFFER] = {0};

    // create the server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {   
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // bind socket
    address.sin_family = AF_INET; // ipv4
    address.sin_addr.s_addr = INADDR_ANY; // liste 0.0.0.0 interfaces
    address.sin_port = htons(PORT); // convert to network byte order

    // bind the server socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        exit(1);
    }

    // create the queues
    // listen for clients, with 10 backlog (10 connections in accept queue)
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // we loop forever to accept new connections
    while (1) {
        printf("Waiting for a new connection...\n");

        // accept a client connection client_fd = connection
        // if the accept queue is empty, we are stuck here
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("New connection accepted\n");

        // read the data from the OS receive buffer to the application buffer
        // this is essentially reading the HTTP request
        // don't bite more than you chew APP_MAX_BUFFER
        read(client_fd, buffer, APP_MAX_BUFFER);

        // print the request
        printf("Request: %s\n", buffer);

        // we send the request by writing to the socket send buffer in the OS
        char *http_response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 13\n\nHello, World!";
        // write to the socket
        // send queue os
        write(client_fd, http_response, strlen(http_response));

        // close the client socket (terminate the TCP connection)
        close(client_fd);
    }
}
