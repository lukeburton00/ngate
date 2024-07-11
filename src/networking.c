#include "networking.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include "../include/config.h"

struct session *create_session()
{
    struct session *session = malloc(sizeof(struct session));
    if (!session)
    {
        perror("malloc session\n");
        return NULL;
    }

    return session;
}

void delete_session(struct session *session)
{
    if (session)
    {
        free(session);
    }
}

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
    int sockfd = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);

    if (sockfd < 0)
    {
        printf("socket error: %s \n", strerror(errno));
        return -1;
    }

    return sockfd;
}

int bind_socket(int sockfd, struct addrinfo *addrinfo)
{
    if (bind(sockfd, addrinfo->ai_addr, addrinfo->ai_addrlen) < 0)
    {
        close(sockfd);
        freeaddrinfo(addrinfo);
        return -1;
    }

    freeaddrinfo(addrinfo);
    return sockfd;
}


int listen_on_socket(int sockfd, int backlog_len)
{
    if (sockfd < 0)
    {
        return -1;
    }

    if (listen(sockfd, backlog_len) < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        close(sockfd);
        return -1;
    }
    return 0;
}
