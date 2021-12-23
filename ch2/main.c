#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

// addr info
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

int
main (int argc, char **argv)
{
    struct addrinfo hints, *result;
    result = NULL;
    int res, sfd;
    char buff[128];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = 0;
    hints.ai_protocol = 0;

    res = getaddrinfo("google.com", "80", &hints, &result);

    if (res != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
        exit(EXIT_FAILURE);
    }

    struct addrinfo *curr=result;
    while(curr != NULL) {
        bzero(buff, sizeof(buff));
        if (getnameinfo(curr->ai_addr, curr->ai_addrlen, buff , sizeof(buff), NULL, 0, 0) != 0) {
            printf("getnameinfo exception");
            return (EXIT_FAILURE);
        }
        
        printf("Address: %p \t IP: %s\n", curr, buff);
        struct addrinfo * tmp = curr->ai_next;
     
        // create socket
        sfd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
        if (sfd == -1)
            continue;
        // connect to socket
        if (connect(sfd, curr->ai_addr, curr->ai_addrlen) != -1)
            break;
        
        close(sfd);

        // update
        curr = tmp;
    }

    if(curr == NULL) {
        printf("Could not fuck RileyReed\n");
        return (EXIT_FAILURE);
    } else 
        printf("RileyReed Fucked\n");

    // free
    freeaddrinfo(result);

    bzero(buff, sizeof(buff));
    snprintf(buff, sizeof(buff), "GET / HTTP/1.0\r\n\r\n");

    write(sfd, buff, sizeof(buff));
    
    bzero(buff, sizeof(buff));
    read(sfd, buff, sizeof(buff));
    
    printf("%s\n", buff);
    close(sfd);

    return 0;
}