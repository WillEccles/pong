#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

/*
 * Config options
 */

// The message to send to the server (overridden by command line arg,
// if available)
#define MESSAGE     "ping!"

// Size of the buffer for reading from clients
#define CONF_BFRLEN 4096

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s IP PORT [msg]\n"
                "\tIP\tThe IP of the server\n"
                "\tPORT\tThe port of the server\n"
                "\tmsg\tThe message to send when data is received"
                "\n\t\t(optional; default is " MESSAGE ")\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    const char* message = (argc >= 4) ? argv[3] : MESSAGE;

    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));

    if (connect(sockfd, (struct sockaddr*)&addr,
                sizeof(struct sockaddr_in)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("\033[32;1mConnected to %s:%hd\033[m\n",
            argv[1], (uint16_t)atoi(argv[2]));
    printf("Sending... ");

    char inbuf[CONF_BFRLEN+1] = {0};

    if (write(sockfd, message, strlen(message)) < 0) {
        printf("\033[31;1mfailed!\033[m\n");
        perror("write");
        exit(EXIT_FAILURE);
    }

    printf("\033[32;1msucceeded\033[m\n");

    printf("Reading... ");

    ssize_t rbytes;
    if ((rbytes = read(sockfd, inbuf, CONF_BFRLEN)) < 0) {
        printf("\033[31;1mfailed!\033[m\n");
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("\033[32;1m%zd bytes\033[m\n", rbytes);

    if (rbytes > 0) {
        printf("Received: %s\n", inbuf);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
