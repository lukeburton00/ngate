#include "request.h"
#include "../include/networking.h"

Session *parse_request(Session *session)
{
    int clientfd = *(int *)session->clientfd;

    char request[1024];
    memset(request, 0, sizeof(request));

    ssize_t bytes_received = recv(clientfd, request, sizeof(request), 0);
    if (bytes_received < 0)
    {
        printf("recv error: %s\n", strerror(errno));
        close(clientfd);
        delete_session(session);
        return NULL;
    }

    sscanf(request, "%s %s %s", session->method, session->path, session->protocol);

    return session;
}
