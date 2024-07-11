#include "../include/config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct config create_config(int argc, char **argv)
{
    char *default_port = "3000";

    struct config config;
    config.port = NULL;

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-p") == 0) && i + 1 < argc)
        {
            config.port = argv[i + 1];
        }

        if ((strcmp(argv[i], "-h") == 0))
        {
            printf("Usage: %s [-p port] [-c config_file] [-h]\n", argv[0]);
            exit(0);
        }
    }

    if (config.port == NULL)
    {
        printf("No port specified. Default is %s\n", default_port);
        config.port = default_port;
    }

    return config;
}
