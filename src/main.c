#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "../include/networking.h"
#include "../include/config.h"
#include "../include/request.h"
#include "../include/response.h"

volatile sig_atomic_t stop = 0;

void handle_signal(int signal) {
    stop = 1;
}

int main(int argc, char* argv[])
{
    struct sigaction act;
    act.sa_handler = handle_signal;
    sigaction(SIGINT, &act, NULL);

    struct config config = create_config(argc, argv);

    struct addrinfo *info = get_info(config.port);
    if (!info)
    {
        printf("Addrinfo error\n");
    }

    int sockfd = get_socket(info);
    if (sockfd < 0)
    {
        printf("Socket error\n");
    }

    if (bind_socket(sockfd, info) < 0)
    {
        printf("Bind error\n");
    }

    if (listen_on_socket(sockfd, 10) < 0)
    {
        printf("Listen error\n");
        return -1;
    }

    printf("nGate is listening on port %s\n", config.port);

    while (!stop)
    {
        struct session *session = create_session();
        struct sockaddr_storage clientaddr;
        socklen_t addrlen = sizeof(clientaddr);

        session->clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
        if ((session->clientfd < 0))
        {
            delete_session(session);
            printf("accept error: %s\n", strerror(errno));
            continue;
        }

        if (parse_request(session) < 0)
        {
            delete_session(session);
            printf("Parse request error\n");
            continue;
        }

        printf("%s %s %s\n", session->method, session->path, session->protocol);
        // printf("%s\n", session->full_request);

        char *response = (char *)malloc(MAX_RESPONSE_SIZE * sizeof(char));
        if (response == NULL)
        {
            fprintf(stderr, "Malloc response error\n");
            close(session->clientfd);
            delete_session(session);
            continue;
        }

        get_response(response);

        if (send(session->clientfd, response, 1024 - 1, 0) < 0)
        {
            printf("send error: %s\n", strerror(errno));
            free(response);
        }

        free(response);
        close(session->clientfd);
        delete_session(session);
    }

    close(sockfd);

    return 0;
}
