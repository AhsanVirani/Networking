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
#include <netdb.h>
#include <arpa/inet.h>

// write
#include <unistd.h>

// defines
#define PORT    "80"
#define MAX_LEN 1024
#define STR_LEN 50
// error handle definition
#define handle_error(msg) { perror(msg); exit(EXIT_FAILURE); }

// buffer for getting IP addr
char str[STR_LEN];
// HTTP GET REQUEST
const char BUFF[]      = "GET / HTTP/1.0\r\n\r\n";
char response[MAX_LEN] = {0};

int 
main (int argc, char **argv) 
{

    // checking arguments given correctly
    if (argc != 2)
        handle_error("Please provide server IP");

    // setting up
    int fd;
    struct addrinfo * result, * rp;
    struct addrinfo hints;

    // bzero'ing the created structs
    bzero(&hints, sizeof(hints));
    bzero(&result, sizeof(result));
    bzero(&rp, sizeof(rp));
    
    // setting up hints
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = 0;
    hints.ai_protocol = 0;
    
    // gets the addr for dns
    if ( (getaddrinfo(argv[1], PORT, &hints, &result)) != 0 )
        handle_error("failed to get addr info");

    // printing ip's returned
    for (rp = result; rp != NULL; rp = rp->ai_next) {

        // connect to socket 
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        // check if fd
        if(fd == -1)
            continue;   // failed (retry)

        printf("Socket connection made with descriptor: %d\n", fd);
     
        // establishing a connection to a secure server
        if (connect(fd, rp->ai_addr, rp->ai_addrlen) != -1) 
            break;

        // close fd after work done
        close(fd);
    }    
    
    if (rp == NULL) {
        handle_error("Failed to establish connection to server");
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }

    // writing request to server
    if (write(fd, BUFF, sizeof(BUFF)) <= 0) 
        handle_error("write failed");

    if (read(fd, response, sizeof(response)) <= 0) 
        handle_error("read failed");
    
    // printing the response on success
    printf("Response by server:\n%s\n", response);

    // free result
    freeaddrinfo(result);

    return 0;
}