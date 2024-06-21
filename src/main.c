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

void handle_connection(AppContext *context, Session *session)
{
    if (parse_request(session) < 0)
    {
        return;
    }

    printf("%s %s %s\n", session->method, session->path, session->protocol);
    printf(session->full_request);

    char *response = (char *)malloc(MAX_RESPONSE_SIZE * sizeof(char));
    if (response == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for request\n");
        close(session->clientfd);
        delete_session(session);
        return;
    }

    get_response(response, context, session);

    if (send(session->clientfd, response, 1024 - 1, 0) < 0)
    {
        printf("send error: %s\n", strerror(errno));
        free(response);
    }

    free(response);
    close(session->clientfd);
    delete_session(session);
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

    AppContext context = configure_context(argc, argv);

    if (begin_listen(&context) < 0)
    {
        printf("begin_listen error\n");
        return -1;
    }

    printf("nGate is listening on port %s\n", context.port);

    while (!stop)
    {
        Session *session = create_session();
        if (accept_connection(&context, session) < 0)
        {
            if (stop) 
            {
                break;
            }
            continue;
        }

        handle_connection(&context, session);
    }

    close(context.sockfd);

    return 0;
}