#pragma once

#include <netdb.h>

typedef struct AppContext AppContext;

int begin_listen(AppContext *context);
int get_bound_socket(const char *port);
static int populate_servinfo(const char *port, struct addrinfo **servinfo);
static int get_socket_fd(struct addrinfo *servinfo);
static int bind_socket(int fd, struct addrinfo *servinfo);