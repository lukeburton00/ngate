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

volatile sig_atomic_t stop = 0;

void handle_signal(int signal) {
    stop = 1;
}

void * handle_connection(void* connection)
{
    int clientfd = *(int*)connection;
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

    int backlog_len = 10;
    const char *PORT = "3000";

    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        printf("getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if (sockfd < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        freeaddrinfo(servinfo);
        return 1;
    }

    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        freeaddrinfo(servinfo);
        close(sockfd);
        return 1;
    }

    freeaddrinfo(servinfo);

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        printf("setsockopt error: %s\n", strerror(errno));
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, backlog_len) < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        close(sockfd);
        return 1;
    }

    printf("nGate is listening on port %s\n", PORT);

    while (!stop)
    {
        struct sockaddr_storage clientaddr;
        socklen_t addrlen = sizeof(clientaddr);

        int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
        if (clientfd < 0)
        {
            if (errno == EINTR && stop) {
                break;
            }
            printf("accept error: %s\n", strerror(errno));
            continue;
        }

        int *pclient = malloc(sizeof(int));
        if (pclient == NULL)
        {
            printf("malloc error, closing connection\n");
            close(clientfd);
            continue;
        }
        *pclient = clientfd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_connection, pclient)!= 0)
        {
            printf("pthread_create error, closing connection\n");
            free(pclient);
            close(clientfd);
            continue;
        }

        pthread_detach(thread);
    }

    close(sockfd);

    return 0;
}