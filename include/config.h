#pragma once

typedef struct AppContext 
{
    char *port;
    int sockfd;
} AppContext;

AppContext configure_context(int argc, char **argv);