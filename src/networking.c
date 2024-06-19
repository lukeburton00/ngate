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

Session *create_session()
{
    Session *session = malloc(sizeof(Session));
    if (!session)
    {
        perror("malloc session\n");
        return NULL;
    }

    return session;
}

void delete_session(Session *session)
{
    if (session)
    {
        free(session);
    }
}

int accept_connection(AppContext *context, Session *session)
{        
    struct sockaddr_storage clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    session->clientfd = accept(context->sockfd, (struct sockaddr *)&clientaddr, &addrlen);
    if (errno == EINTR) 
    {
        delete_session(session);
        return -1;
    }

    if ((session->clientfd < 0))
    {
        delete_session(session);
        printf("accept error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int begin_listen(AppContext *context)
{
    context->sockfd = get_bound_socket(context->port);
    if (context->sockfd < 0)
    {
        return -1;
    }

    if (listen(context->sockfd, 10) < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        close(context->sockfd);
        return -1;
    }
    return 0;
}

int get_bound_socket(const char *port)
{
    struct addrinfo *servinfo;

    if (populate_servinfo(port, &servinfo) < 0)
    {
        freeaddrinfo(servinfo);
        return -1;
    }

    int sockfd = get_socket_fd(servinfo);
    if (sockfd < 0)
    {
        freeaddrinfo(servinfo);
        return -1;
    }

    if (bind_socket(sockfd, servinfo) < 0)
    {
        close(sockfd);
        freeaddrinfo(servinfo);
        return -1;
    }

    freeaddrinfo(servinfo);
    return sockfd;
}

int populate_servinfo(const char *port, struct addrinfo **servinfo)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((status = getaddrinfo(NULL, port, &hints, servinfo)) != 0)
    {
        printf("getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }
    return 0;
}

int get_socket_fd(struct addrinfo *servinfo)
{
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if (sockfd < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    return sockfd;
}

int bind_socket(int fd, struct addrinfo *servinfo)
{
    if (bind(fd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}