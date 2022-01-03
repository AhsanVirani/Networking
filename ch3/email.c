#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
 
// socket
#include <sys/types.h>
#include <sys/socket.h>
// email protocol imports
#include <ctype.h>
#include <stdarg.h>

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
void send_format(SOCKET server, const char *text, ...) {
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
int parse_response(const char *response) {
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
void wait_on_response(SOCKET server, int expecting) {
    char response[MAXRESPONSE+1];
    char * p=response;
    char *end= response + MAXRESPONSE;

    int code = 0;
}

int main(void) {
    const char *str = "300-my message is\n250 here okay\r\n\\0";
    printf("%d\n", parse_response(str));
}