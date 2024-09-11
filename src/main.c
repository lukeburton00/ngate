#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "../include/config.h"
#include "../include/networking.h"

volatile sig_atomic_t stop = 0;
void handle_signal(int signal) { stop = 1; }

int setup_server(const char *port)
{
    struct addrinfo *info = get_info(port);
    if (!info)
    {
        fprintf(stderr, "Addrinfo error\n");
        return -1;
    }

    int sockfd = get_socket(info);
    if (sockfd < 0)
    {
        fprintf(stderr, "Socket error\n");
        return -1;
    }

    if (bind_socket(sockfd, info) < 0)
    {
        fprintf(stderr, "Bind error\n");
        close(sockfd);
        return -1;
    }

    if (listen_on_socket(sockfd, 10) < 0)
    {
        fprintf(stderr, "Listen error\n");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int get_request(int clientfd, char *buffer)
{
    if (read_from_socket(clientfd, buffer) < 0)
    {
        fprintf(stderr, "Receive request error\n");
        return -1;
    }

    char method[16], path[256], protocol[16];
    if (sscanf(buffer, "%s %s %s\n", method, path, protocol) < 0)
    {
        perror("Scan request fields error\n");
        return -1;
    }

    printf("%s %s %s\n", method, path, protocol);
    return 0;
}

int get_response(char *buffer, char *request, const char *proxy_port)
{
    struct addrinfo *proxy_info = get_info(proxy_port);
    if (!proxy_info)
    {
        fprintf(stderr, "Addrinfo error\n");
        return -1;
    }

    int proxy_fd = get_socket(proxy_info);
    if (proxy_fd < 0)
    {
        fprintf(stderr, "Socket error\n");
        return -1;
    }

    if (connect_to_socket(proxy_fd, proxy_info) < 0)
    {
        fprintf(stderr, "Connect error\n");
        close(proxy_fd);
        return -1;
    }

    if (send_on_socket(proxy_fd, request) < 0)
    {
        fprintf(stderr, "Send error\n");
        close(proxy_fd);
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    if (set_timeout_sockopt(proxy_fd, &timeout) < 0)
    {
        fprintf(stderr, "timeout sockopt error");
        close(proxy_fd);
        return -1;
    }

    if (read_from_socket(proxy_fd, buffer) < 0)
    {
        if ((errno != EAGAIN) && (errno != EWOULDBLOCK))
        {
            fprintf(stderr, "Gateway timed out\n");
        }

        fprintf(stderr, "Read error\n");
        close(proxy_fd);
        return -1;
    }

    close(proxy_fd);
    return 0;
}

int handle_client(int clientfd, const char *proxy_port)
{
    char *request = malloc(MAX_REQUEST_SIZE * sizeof(char));
    if (!request)
    {
        fprintf(stderr, "Malloc request error\n");
        close(clientfd);
        return -1;
    }
    memset(request, 0, MAX_REQUEST_SIZE * sizeof(char));

    if (get_request(clientfd, request) < 0)
    {
        fprintf(stderr, "Failed to receive client request.\n");
        free(request);
        close(clientfd);
        return -1;
    }

    char *response = (char *)malloc(MAX_RESPONSE_SIZE * sizeof(char));
    if (response == NULL)
    {
        fprintf(stderr, "Malloc response error\n");
        free(request);
        close(clientfd);
        return -1;
    }
    memset(response, 0, MAX_RESPONSE_SIZE * sizeof(char));

    if (get_response(response, request, proxy_port) < 0)
    {
        fprintf(stderr, "Failed to receive server response.\n");
        // TODO: return 502 Gateway Error
        free(request);
        free(response);
        close(clientfd);
        return -1;
    }

    if (send_on_socket(clientfd, response) < 0)
    {
        fprintf(stderr, "Send error\n");
        free(request);
        free(response);
        close(clientfd);
        return -1;
    }

    free(request);
    free(response);
    close(clientfd);
    return 0;
}

int main(int argc, char* argv[])
{
    struct sigaction act = {0};
    act.sa_handler = handle_signal;
    sigaction(SIGINT, &act, NULL);

    struct config config = create_config(argc, argv);

    int serverfd = setup_server(config.port);
    if (serverfd < 0)
    {
        fprintf(stderr, "Failed to create server.");
        return -1;
    }

    printf("nGate is listening on port %s\n", config.port);

    while (!stop)
    {
        int clientfd = accept_on_socket(serverfd);
        if (clientfd < 0)
        {
            if (stop)
            {
                printf("Exiting...\n");
                break;
            }

            fprintf(stderr, "Accept error: %s\n", strerror(errno));
            continue;
        }

        if (handle_client(clientfd, config.proxy_port) < 0)
            continue;
    }

    close(serverfd);

    return 0;
}
