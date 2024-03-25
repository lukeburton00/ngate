#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int main(int argc, char** argv)
{
    printf("Initializing server...\n");

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("webs failed to create socket\n");
        return 1;
    }

    printf("TCP Socket Created.\n");

    return 0;
}