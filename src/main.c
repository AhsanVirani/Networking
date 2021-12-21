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
    int sock, res, fd;
    char *port;
    struct sockaddr_in myAddr;

    sock = socket (AF_INET, SOCK_STREAM, 0);

    bzero (&myAddr, sizeof(myAddr));

    port = "8000";
    myAddr.sin_family = AF_INET;
    myAddr.sin_port   = htons( (uint16_t)strtoul( port, NULL, 10 ) );

    res = bind(sock, (struct sockaddr *) &myAddr, sizeof(myAddr));
    
    // 1 failure, 0 success
    assert (!res);

    res = listen(sock, 50);
    assert (!res);

    while(true) {
        
        struct sockaddr userAddr;
        socklen_t userAddrsize;
        userAddrsize = sizeof(userAddr);

        bzero (&userAddr, sizeof(userAddr));

        fd = accept(sock, (struct sockaddr *) &userAddr, &userAddrsize);
        printf("Got a connection\n");
        fflush(stdout);

        assert(fd != -1);
        
        write (fd, response, sizeof(response));

        close ( fd );
    }

    return (0);
}