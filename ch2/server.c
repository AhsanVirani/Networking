/*
Written By: Ahsan Muhammad
Dated     : 28 December 2021
Topic     : Socket Client in C 
*/

// standard imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// socket imports
#include <sys/types.h>          
#include <sys/socket.h>

// sockaddr
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

// write
#include <unistd.h>

// defines
#define PORT    8000
#define MAX_LEN 1024
#define BACKLOG 10
// error handle definition
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

char client_message[MAX_LEN];
// Response
char response[] = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<html>"
        "<head>"
            "<title>PHP is the best!!!</title>"
        "</head>"
        "<body>"
            "<h1>Pornhub with Riley Reed</h1>"
        "</body>"
    "</html>";

int 
main (int argc, char **argv) 
{

    int fd, client_sock;
    struct sockaddr_in addr;
    size_t addr_size = sizeof(addr);

    // bzero'ing the addr
    bzero(&addr, sizeof(addr));

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port    = htons(PORT);
    addr.sin_family  = AF_INET;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    // check if fd
    if(fd <= 0)
        handle_error("socket failed");
    printf("Socket connection made with descriptor: %d\n", fd);

    // bind the socket to port
    if (bind(fd, (const struct sockaddr *) &addr, addr_size) == -1)
        handle_error("Binding failed");

    // Listen 
    if (listen(fd, BACKLOG) == -1)
        handle_error("Listen failed");

    // Now we will accept connection from clients and reply to them
    while(1) {
        client_sock = accept(fd, (struct sockaddr *) &addr, (socklen_t *) &addr_size);
        if (client_sock == -1) {
            printf("Failed to accept connection from client");
            continue;
        }
        printf("Got a connection");
        // else read message
        if (recv(client_sock, (char *) client_message, MAX_LEN, 0) == -1) {
            printf("Failed to read client ,message");
            continue;
        }

        //Send the message back to client
		send(client_sock , (char *) response , sizeof(response), 0);
        close(client_sock);
    }
    
    close(fd);

    return 0;
}