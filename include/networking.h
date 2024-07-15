#pragma once

#define MAX_REQUEST_SIZE 1000000
#define MAX_RESPONSE_SIZE 1000000

struct addrinfo *get_info(const char *port);
int get_socket(struct addrinfo *addrinfo);
int bind_socket(int sockfd, struct addrinfo *info);
int listen_on_socket(int sockfd, int backlog_len);
int accept_on_socket(int sockfd);
int read_from_socket(int sockfd, char *buffer);
int send_on_socket(int sockfd, char *message);