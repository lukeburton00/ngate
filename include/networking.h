#pragma once

struct session
{
    int clientfd;
    char method[16], path[256], protocol[16], full_request[1024];
};

struct session *create_session();
void delete_session(struct session *session);

typedef struct AppContext AppContext;

struct addrinfo *get_info(const char *port);
int get_socket(struct addrinfo *addrinfo);
int bind_socket(int sockfd, struct addrinfo *info);
int listen_on_socket(int sockfd, int backlog_len);
