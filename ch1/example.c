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

#define PORT 80
#define MAX_LEN 1024

// HTTP GET REQUEST
const char BUFF[] = "GET / HTTP/1.0\r\n\r\n";

int main (int argc, char **argv) {

    int fd;
    struct sockaddr_in addr;
    size_t addr_size = sizeof(addr);

    bzero(&addr, sizeof(addr));
    // filling sockaddr in
    if(inet_pton(AF_INET, argv[1], &(addr.sin_addr)) <= 0) { // IPv4
        printf("inet_pton failed");
        return EXIT_FAILURE;
    }

    addr.sin_port = htons(PORT);
    addr.sin_family   = AF_INET;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    // check if fd
    if(fd == -1) {
        printf("socket failed");
        return EXIT_FAILURE; 
    }

    printf("Your socket file descriptor is: %d\n", fd);

    // establishing a connection to a secure server
    if(connect(fd, (struct sockaddr *) &addr, addr_size) == -1) {
        printf("connection failed");
        return EXIT_FAILURE;
    }
    
    printf("%zu\n", sizeof(BUFF));

    if(write(fd, BUFF, sizeof(BUFF)) != 19) {
        printf("write failed");
        return EXIT_FAILURE;
    }

    char BUFF_NEW[MAX_LEN] = {0};
    if(read(fd, BUFF_NEW, sizeof(BUFF_NEW)) == -1) {
        printf("read failed");
        return EXIT_FAILURE;
    }
    printf("%s\n", BUFF_NEW);

    return 0;
}