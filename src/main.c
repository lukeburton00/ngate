#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include "../include/networking.h"
#include "../include/config.h"

volatile sig_atomic_t stop = 0;

void handle_signal(int signal) {
    stop = 1;
}

void * handle_connection(void *connection)
{
    int clientfd = *(int *)connection;
    free(connection);

    char request[1024];
    memset(request, 0, sizeof(request));

    ssize_t bytes_received = recv(clientfd, request, sizeof(request), 0);
    if (bytes_received < 0)
    {
        printf("recv error: %s\n", strerror(errno));
        close(clientfd);
        return NULL;
    }

    char method[16], path[256], protocol[16];
    sscanf(request, "%s %s %s", method, path, protocol);
    printf("%s %s %s\n", method, path, protocol);

    char response[1024];
    memset(response, 0, sizeof(response));
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello World!</h1></body></html>\r\n");

    if (send(clientfd, response, sizeof(response) - 1, 0) < 0)
    {
        printf("send error: %s\n", strerror(errno));
    }

    close(clientfd);
    return NULL;
}

int main(int argc, char* argv[])
{
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    AppContext context;

    context.port = "3000";

    if (begin_listen(&context) < 0)
    {
        printf("begin_listen error\n");
        return -1;
    }

    printf("nGate is listening on port %s\n", context.port);

    while (!stop)
    {
        struct sockaddr_storage clientaddr;
        socklen_t addrlen = sizeof(clientaddr);

        int *clientfd = malloc(sizeof(int));
        if (!clientfd)
        {
            perror("malloc");
            continue;
        }

        *clientfd = accept(context.sockfd, (struct sockaddr *)&clientaddr, &addrlen);
        if (*clientfd < 0)
        {
            free(clientfd);
            if (errno == EINTR && stop) 
            {
                break;
            }
            printf("accept error: %s\n", strerror(errno));
            continue;
        }

        pthread_t thread;

        if (pthread_create(&thread, NULL, handle_connection, clientfd)!= 0)
        {
            printf("pthread_create error, closing connection\n");
            close(*clientfd);
            free(clientfd);
            continue;
        }

        pthread_detach(thread);
    }

    close(context.sockfd);

    return 0;
}