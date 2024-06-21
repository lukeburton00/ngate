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

    char *request = (char *)malloc(MAX_REQUEST_SIZE * sizeof(char));
    if (request == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for request\n");
        close(clientfd);
        delete_session(session);
        return -1;
    }

    if (recv(clientfd, request, MAX_REQUEST_SIZE - 1, 0) < 0)
    {
        printf("recv error: %s\n", strerror(errno));
        close(clientfd);
        delete_session(session);
        free(request);
        return -1;
    }

    memcpy(session->full_request, request, sizeof(session->full_request));
    sscanf(request, "%s %s %s", session->method, session->path, session->protocol);

    free(request);

    return 0;
}