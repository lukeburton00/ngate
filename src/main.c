#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "../include/config.h"
#include "../include/networking.h"

volatile sig_atomic_t stop = 0;
void handle_signal(int signal) { stop = 1; }

int main(int argc, char* argv[])
{
    struct sigaction act = {0};
    act.sa_handler = handle_signal;
    sigaction(SIGINT, &act, NULL);

    struct config config = create_config(argc, argv);

    struct addrinfo *info = get_info(config.port);
    if (!info)
    {
        fprintf(stderr, "Addrinfo error\n");
        return -1;
    }

    int sockfd = get_socket(info);
    if (sockfd < 0)
    {
        fprintf(stderr, "Socket error\n");
        return -1;
    }

    if (bind_socket(sockfd, info) < 0)
    {
        fprintf(stderr, "Bind error\n");
        close(sockfd);
        return -1;
    }

    if (listen_on_socket(sockfd, 10) < 0)
    {
        fprintf(stderr, "Listen error\n");
        close(sockfd);
        return -1;
    }

    printf("nGate is listening on port %s\n", config.port);

    while (!stop)
    {
        int clientfd = accept_on_socket(sockfd);
        if (clientfd < 0)
        {
            close(clientfd);
            if (stop)
            {
                printf("Exiting...\n");
                break;
            }

            fprintf(stderr, "Accept error: %s\n", strerror(errno));
            continue;
        }

        char *request = malloc(MAX_REQUEST_SIZE * sizeof(char));
        if (!request)
        {
            fprintf(stderr, "Malloc request error\n");
            close(clientfd);
            continue;
        }
        memset(request, 0, MAX_REQUEST_SIZE * sizeof(char));

        if (read_from_socket(clientfd, request) < 0)
        {
            fprintf(stderr, "Receive request error\n");
            free(request);
            close(clientfd);
            continue;
        }

        char method[16], path[256], protocol[16];
        sscanf(request, "%s %s %s\n", method, path, protocol);
        printf("%s %s %s\n", method, path, protocol);

        char *response = (char *)malloc(MAX_RESPONSE_SIZE * sizeof(char));
        if (response == NULL)
        {
            fprintf(stderr, "Malloc response error\n");
            free(request);
            close(clientfd);
            continue;
        }
        memset(response, 0, MAX_RESPONSE_SIZE * sizeof(char));        

        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello World!</h1></body></html>\r\n");

        if (send_on_socket(clientfd, response) < 0)
        {
            fprintf(stderr, "Send error\n");
            free(request);
            free(response);
            close(clientfd);
            continue;
        }

        free(request);
        free(response);
        close(clientfd);
    }

    close(sockfd);

    return 0;
}
