#include "request.h"
#include "../include/networking.h"

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

int parse_request(Session *session)
{
    int clientfd = session->clientfd;

    char request[1024];
    memset(request, 0, sizeof(request));

    ssize_t bytes_received = recv(clientfd, request, sizeof(request), 0);
    if (bytes_received < 0)
    {
        printf("recv error: %s\n", strerror(errno));
        close(clientfd);
        delete_session(session);
        return -1;
    }

    memcpy(session->full_request, request, sizeof(session->full_request));
    sscanf(request, "%s %s %s", session->method, session->path, session->protocol);

    return 0;
}
