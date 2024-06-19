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

volatile sig_atomic_t stop = 0;

void handle_signal(int signal) {
    stop = 1;
}

void handle_connection(Session *session)
{
    if (parse_request(session) < 0)
    {
        return;
    }

    printf("%s %s %s\n", session->method, session->path, session->protocol);
    printf(session->full_request);

    char response[1024];
    memset(response, 0, sizeof(response));
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello World!</h1></body></html>\r\n");

    if (send(session->clientfd, response, sizeof(response) - 1, 0) < 0)
    {
        printf("send error: %s\n", strerror(errno));
    }

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

        handle_connection(session);
    }

    close(context.sockfd);

    return 0;
}