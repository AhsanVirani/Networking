#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <unistd.h>

// generic conversion of port (htons)
#include <arpa/inet.h>
// socket imports
#include <sys/types.h>         
#include <sys/socket.h>

 #define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

char response[] = 
    "HTTP/1.1 200 OK\n"
    "Content-Type: text/html\n"
    "\n"
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
    int sock, cfd;
    char *port;
    struct sockaddr_in myAddr;

    sock = socket (AF_INET, SOCK_STREAM, 0);

    bzero (&myAddr, sizeof(myAddr));

    port = "8000";
    myAddr.sin_family = AF_INET;
    myAddr.sin_port   = htons( (uint16_t)strtoul( port, NULL, 10 ) );

    if( bind(sock, (struct sockaddr *) &myAddr, sizeof(myAddr)) == -1)
        handle_error("bind");
    
    // 1 failure, 0 success
    if (listen(sock, 50) == -1)
        handle_error("listen");

    while(true) {
        
        struct sockaddr userAddr;
        socklen_t userAddrsize;
        userAddrsize = sizeof(userAddr);

        bzero (&userAddr, sizeof(userAddr));

        cfd = accept(sock, (struct sockaddr *) &userAddr, &userAddrsize);

        if (cfd == -1)    
            handle_error("accept");
        
        printf("Got a connection\n");
        fflush(stdout);
        
        write (cfd, response, sizeof(response) - 1);

        close ( cfd );
    }

    return (0);
}