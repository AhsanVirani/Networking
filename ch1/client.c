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
#define PORT 80
#define MAX_LEN 1024
// error handle definition
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

// HTTP GET REQUEST
const char BUFF[]      = "GET / HTTP/1.0\r\n\r\n";
char response[MAX_LEN] = {0};


int 
main (int argc, char **argv) 
{

    // checking arguments given correctly
    if (argc != 2)
        handle_error("Please provide server IP");

    int fd;
    struct sockaddr_in addr;
    size_t addr_size = sizeof(addr);

    // bzero'ing the addr
    bzero(&addr, sizeof(addr));
    
    // filling sockaddr in
    if (inet_pton(AF_INET, argv[1], &(addr.sin_addr)) <= 0) // IPv4
        handle_error("inet_pton failed");


    addr.sin_port    = htons(PORT);
    addr.sin_family  = AF_INET;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    // check if fd
    if(fd <= 0)
        handle_error("socket failed");

    printf("Socket connection made with descriptor: %d\n", fd);

    // establishing a connection to a secure server
    if (connect(fd, (struct sockaddr *) &addr, addr_size) == -1) 
        handle_error("connection failed");

    
    // writing request to server
    if (write(fd, BUFF, sizeof(BUFF)) <= 0) 
        handle_error("write failed");

    if (read(fd, response, sizeof(response)) <= 0) 
        handle_error("read failed");
    
    // printing the response on success
    printf("Response by server:\n%s\n", response);

    return 0;
}