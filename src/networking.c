#include "../include/networking.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

struct addrinfo *get_info(const char *port)
{
    struct addrinfo *addrinfo;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((getaddrinfo(NULL, port, &hints, &addrinfo)) != 0)
    {
        printf("getaddrinfo: %s\n", gai_strerror(status));
        return NULL;
    }

    return addrinfo;
}

int get_socket(struct addrinfo *addrinfo)
{
    if (!addrinfo)
    {
        fprintf(stderr, "addrinfo invalid\n");
        return -1;
    }

    int sockfd = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);

    if (sockfd < 0)
    {
        perror("socket error\n");
        return -1;
    }

    return sockfd;
}

int bind_socket(int sockfd, struct addrinfo *addrinfo)
{
    if (sockfd < 0)
    {
        fprintf(stderr, "sockfd invalid\n");
        return -1;
    }

    if (!addrinfo)
    {
        fprintf(stderr, "addrinfo invalid\n");
        return -1;
    }

    if (bind(sockfd, addrinfo->ai_addr, addrinfo->ai_addrlen) < 0)
    {
        perror("bind error\n");
        freeaddrinfo(addrinfo);
        return -1;
    }

    freeaddrinfo(addrinfo);
    return sockfd;
}

int connect_to_socket(int sockfd, struct addrinfo *addrinfo)
{
    if (connect(sockfd, addrinfo->ai_addr, addrinfo->ai_addrlen) < 0)
    {
        perror("connect error\n");
        freeaddrinfo(addrinfo);
        return -1;
    }

    freeaddrinfo(addrinfo);
    return 0;
}


int listen_on_socket(int sockfd, int backlog_len)
{
    if (sockfd < 0)
    {
        fprintf(stderr, "sockfd invalid\n");
        return -1;
    }

    if (listen(sockfd, backlog_len) < 0)
    {
        perror("listen error\n");
        return -1;
    }
    return 0;
}

int accept_on_socket(int sockfd)
{
    if (sockfd < 0)
    {
        fprintf(stderr, "sockfd invalid\n");
        return -1;
    }

    struct sockaddr_storage clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
    if ((clientfd < 0))
    {
        // silently fail if SIGINT is received
        if (errno == EINTR)
            return -1;

        perror("accept error\n");
        return -1;
    }

    return clientfd;
}

int read_from_socket(int sockfd, char *buffer)
{
    if (sockfd < 0)
    {
        fprintf(stderr, "sockfd invalid\n");
        return -1;
    }

    if (recv(sockfd, buffer, MAX_REQUEST_SIZE - 1, 0) < 0)
    {
        perror("recv error\n");
        return -1;
    }

    return 0;
}

int send_on_socket(int sockfd, char *message)
{
    if (sockfd < 0)
    {
        fprintf(stderr, "sockfd invalid\n");
        return -1;
    }

    if (send(sockfd, message, strlen(message), 0) < 0)
    {
        perror("send error\n");
        return -1;
    }

    return 0;
}
