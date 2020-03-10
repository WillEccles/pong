#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * Config options
 */

// The response to send to a client (overridden by command line arg,
// if available)
#define RESPONSE    "pong!"

// Size of backlog queue for listening
#define CONF_BACKLOG    5

// IP address to listen at
#define CONF_IP     "127.0.0.1"

// Size of the buffer for reading from clients
#define CONF_BFRLEN 4096

static int sockfd;

void sighandler(int x) {
    (void)x;
    if (sockfd != -1 && sockfd != 0) {
        close(sockfd);
    }
    puts("");
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        fprintf(stderr, "usage: %s PORT [response]\n"
                "\tPORT\tThe port to listen on\n"
                "\tresponse\tThe message to send when data is received"
                " (optional)\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sighandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    const char* response = (argc >= 3) ? argv[2] : RESPONSE;

    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = inet_addr(CONF_IP);
    memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (listen(sockfd, CONF_BACKLOG) != 0) {
        perror("listen");
        return EXIT_FAILURE;
    }

    printf("\033[32;1mServer listening at %s:%hd\033[m\n",
            CONF_IP, (uint16_t)atoi(argv[1]));
    printf("Use ^C to quit.\n");

    int cfd;
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    char inbuf[CONF_BFRLEN+1] = {0};

    for (;;) {
        if ((cfd = accept(sockfd,
            (struct sockaddr*)&clientaddr,
            &len)) == -1) {
            perror("accept");
            continue;
        }
        
        if (0 == fork()) {
            close(sockfd);
            if (read(cfd, inbuf, CONF_BFRLEN) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            printf("\033[33;1mConnection from %s:%hu\033[m\nData: %s\n",
                    inet_ntoa(clientaddr.sin_addr),
                    ntohs(clientaddr.sin_port),
                    inbuf);
            if (-1 == write(cfd, response, strlen(response))) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
        
        close(cfd);
    }

    return EXIT_SUCCESS;
}
