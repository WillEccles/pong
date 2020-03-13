#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

/*
 * Config options
 */

// The response to send to a client (overridden by command line arg,
// if available)
#define RESPONSE    "pong!"

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
    _exit(EXIT_SUCCESS);
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
    
    const char* response = (argc >= 3) ? argv[2] : RESPONSE;

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sighandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr, clientaddr;
    memset(&addr, 0, sizeof(addr));
    memset(&clientaddr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }

    printf("\033[32;1mServer listening on port %hd\033[m\n",
            (uint16_t)atoi(argv[1]));
    printf("Use ^C to quit.\n");

    char buf[CONF_BFRLEN] = {0};
    ssize_t r;
    socklen_t len = sizeof(clientaddr);
    while (1) {
        memset(buf, 0, CONF_BFRLEN);
        if ((r = recvfrom(sockfd, buf, CONF_BFRLEN-1, 0,
                        (struct sockaddr*)&clientaddr, &len)) < 0) {
            if (errno != ECONNRESET) {
                perror("recvfrom");
                close(sockfd);
                return EXIT_FAILURE;
            }
        }
        if (r) {
            printf("\033[33;1mData from %s:%hu\033[m\n%s\n",
                    inet_ntoa(clientaddr.sin_addr),
                    ntohs(clientaddr.sin_port),
                    buf);
            r = sendto(sockfd, response, strlen(response), 0,
                    (struct sockaddr*)&clientaddr, len);
            if (r < 0) {
                if (errno != ECONNRESET) {
                    perror("sendto");
                    close(sockfd);
                    return EXIT_FAILURE;
                }
            }
        }
    }
}
