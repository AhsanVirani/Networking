#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
 
// socket
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
// email protocol imports
#include <ctype.h>
#include <stdarg.h>

// time
#include <time.h>

#define SOCKET      int
#define MAXINPUT    512
#define MAXRESPONSE 1024


// user input to get email server's hostname and recipients address
void get_input(const char *prompt, char *buffer) {
    printf("%s\n", prompt);

    fgets(buffer, MAXINPUT, stdin);
    const int read = strlen(buffer);

    if (read > 0)
        buffer[read -1] = '\0';
}

// send formatted string directly to network
void 
send_format(SOCKET server, const char *text, ...) 
{
    char buffer[MAXINPUT];
    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

    send(server, buffer, strlen(buffer), 0);

    printf("C: %s\n", buffer);

}

// parse response from server
// multiline response followed by '-' after code
// returns parsed response code or 0 if nothing
int 
parse_response(const char *response) 
{
    const char *k = response;
    // looking for 3 digit code
    if (!k[0] || !k[1] || !k[3])
        return 0;
    for (; k[3]; ++k) {
        if (k == response || k[-1] == '\n') {
            if (isdigit(k[0]) && isdigit(k[1]) && isdigit(k[2])) {
                if (k[3] != '-') {
                    if(strstr(k, "\r\n")) {
                        return strtol(k, 0, 10);
                    }
                }
            }
        }
    }
    
    return 0;
}

// waiting to receive a response on the network
void 
wait_on_response(SOCKET server, int expecting) 
{
    char response[MAXRESPONSE+1];
    char *p=response;
    char *end= response + MAXRESPONSE;

    int code = 0;

    do {
        int byte_received = recv(server, p, (end - p), 0);
        if (byte_received < 1) {
            fprintf(stderr, "Connection dropped.\n");
            exit(1);
        }

        p += byte_received;
        *p = 0;

        if (p == end) {
            fprintf(stderr, "Server response too large:\n");
            fprintf(stderr, "%s", response);
            exit(1);
        }

        code = parse_response(response);
    } while (code == 0);

    // printing output to screen
    if (code != expecting) {
        fprintf(stderr, "Error from server:\n");
        fprintf(stderr, "%s", response);
        exit(1);
    }

    printf("%s", response);
}

// creating client to connect to server
SOCKET 
connect_to_host(const char *hostname, const char *port) 
{
    printf("Configuring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;

    if (getaddrinfo(hostname, port, &hints, &peer_address)) {
        printf("getaddrinfo() failed.\n");
        exit(1);
    }

    printf("Remote address is: ");
    
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
                    address_buffer, sizeof(address_buffer),
                    service_buffer, sizeof(service_buffer), NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);

    // hostname and service catered. Now creating socket
    printf("Creating Socket...\n");
    SOCKET server;
    server = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    if(server == -1) {
        printf("socket() failed\n");
        exit(1);
    }

    // make connection to the peer 
    printf("connecting...\n");
    
    if (connect(server, peer_address->ai_addr, peer_address->ai_addrlen)) {
        printf("connect() failed.\n");
        exit(1);
    }

    // freeaddrinfo
    freeaddrinfo(peer_address);

    printf("Connected...\n\n");

    return server;
}

int 
main(void) 
{
    char hostname[MAXINPUT];
    // gets hostname input from user
    get_input("mail server: ", hostname);

    printf("Connecting to host: %s:25\n", hostname);

    SOCKET server = connect_to_host(hostname, "25");

    // Wait after connection for server to respond with 220 code
    wait_on_response(server, 220);

    // Issue HELO command once server is ready and wait for 250 code
    send_format(server, "HELO HONPWC\r\n");
    wait_on_response(server, 250);

    // Ask for user input of sender address
    char sender[MAXINPUT];
    get_input("from: ", sender);
    send_format(server, "MAIL FROM:<%s>\r\n", sender);
    wait_on_response(server, 250);

    // Ask for recipients address
    char recipient[MAXINPUT];
    get_input("to: ", recipient);
    send_format(server, "RCPT TO:<%s>\r\n", recipient);
    wait_on_response(server, 250);

    // Issue data command after sender and receiver specified
    send_format(server, "DATA\r\n");
    wait_on_response(server, 354);

    /*
    Specify now:
        1) subject line
        2) email headers: From, To, and Subject 
    */
   char subject[MAXINPUT];
   get_input("subject: ", subject);
   send_format(server, "FROM:<%s>\r\n", sender);
   send_format(server, "TO:<%s>\r\n", recipient);
   send_format(server, "Subject:%s\r\n", subject);

    // Adding date header - Useful
    time_t t;
    time(&t);

    struct tm *timeinfo;

    timeinfo = gmtime(&t);
    
    char date[128];
    strftime(date, 128, "%a, %d %b %Y %H:%M:%S +0000",
        timeinfo);

    // send formatted date to server
    send_format(server, "Date:%s\r\n", date);

    // send email body delineated by a blank line
    send_format(server, "\r\n");

    // Ready to send email body now
    // send each line. When ready to end mail, send period on a line by itself

    printf("Enter your email text, end with \".\" on a line by itself.\n");

    while(1) {
        char body[MAXINPUT];
        get_input("> ", body);
        send_format(server, "%s\r\n", body);

        if(strcmp(body, ".") == 0) {
            break;
        }
    }
    
    // If the email is accepted server sends code 250
    wait_on_response(server, 250);

    // If accepted, QUIT the connection and check for 221 code - terminating
    send_format(server, "QUIT\r\n");
    wait_on_response(server, 221);

    printf("\nClosing socket...\n");
    close(server);

    printf("Finished.\n");

    return 0;
}