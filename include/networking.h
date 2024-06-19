#pragma once

#include <netdb.h>

typedef struct Session
{
    int clientfd;
    char method[16], path[256], protocol[16], full_request[1024];
} Session;

Session *create_session();
void delete_session(Session *session);

typedef struct AppContext AppContext;

int accept_connection(AppContext *context, Session *session);
int begin_listen(AppContext *context);
static int get_bound_socket(const char *port);
static int populate_servinfo(const char *port, struct addrinfo **servinfo);
static int get_socket_fd(struct addrinfo *servinfo);
static int bind_socket(int fd, struct addrinfo *servinfo);