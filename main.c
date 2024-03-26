#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int port;
    int backlog_len = 128;
    int buffer_size = 1024;
    char req_buffer[buffer_size];
    char response[] = "HTTP/1.0 200 OK\r\n"
    "Server: webs\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>Hello, World!</html>\r\n";

    if (argc < 2)
    {
        printf("No port specified. Default is 8080\n");
        port = 8080;
    }

    else 
    {
        port = atoi(argv[1]);
    }

    // handle port set to 0 (such as when a string is passed to args)
    if (port == 0) 
    {
        printf("No port specified. Default is 8080\n");
        port = 8080;
    }


    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("webs failed to create socket\n");
        return 1;
    }

    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(port);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int len_hostaddr = sizeof(host_addr);

    if (bind(socket_fd, (struct sockaddr *)&host_addr, len_hostaddr) < 0)
    {
        perror("webs failed to bind socket\n");
        return 1;
    }

    if(listen(socket_fd, backlog_len) < 0)
    {
        perror("webs failed to begin listen");
    }

    printf("webs listening on port %d\n", port);

    for(;;)
    {
        int conn_fd = accept(socket_fd, (struct sockaddr *)&host_addr, &len_hostaddr);
        if (conn_fd < 0) 
        { 
            perror("webs detected a request and failed to accept"); 
            continue;
        }

        printf("webs accepted a request");

        int read_req = read(conn_fd, req_buffer, buffer_size);
        if (read_req < 0)
        {
            perror("webs could not read request");
            continue;
        }

        int write_res = write(conn_fd, response, strlen(response));

        close(conn_fd); 
    }

    return 0;
}