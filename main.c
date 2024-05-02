#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
    int backlog_len = 128;
    int buffer_size = 1024;
    char req_buffer[buffer_size];
    char response[] = "HTTP/1.0 200 OK\r\n"
    "Server: webs\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>Hello, World!</html>\r\n";

    const int PORT = 3000; 

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("webs failed to create socket\n");
        return 1;
    }

    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t host_addr_length = sizeof(host_addr);

    struct sockaddr_in client_addr;
    socklen_t client_addr_length = sizeof(client_addr);

    if (bind(socket_fd, (struct sockaddr *)&host_addr, host_addr_length) < 0)
    {
        perror("webs failed to bind socket\n");
        return 1;
    }

    if(listen(socket_fd, backlog_len) < 0)
    {
        perror("webs failed to begin listen");
    }

    printf("webs listening on port %d\n", PORT);

    for(;;)
    {
        int conn_fd = accept(socket_fd, (struct sockaddr *)&host_addr, &host_addr_length);
        if (conn_fd < 0) 
        { 
            perror("webs detected a request and failed to accept"); 
            continue;
        }

        int socket_name = getsockname(conn_fd, (struct sockaddr *)&client_addr, &client_addr_length);
        if (socket_name < 0)
        {
            perror("webs could not retrieve client socket name");
            continue;
        }

        int read_req = read(conn_fd, req_buffer, buffer_size);
        if (read_req < 0)
        {
            perror("webs could not read request");
            continue;
        }
        char method[buffer_size], uri[buffer_size], version[buffer_size];
        sscanf(req_buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), method, version, uri);
        
        int write_res = write(conn_fd, response, strlen(response));

        close(conn_fd); 
    }

    return 0;
}
