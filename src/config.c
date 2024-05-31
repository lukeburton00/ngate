#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

AppContext configure_context(int argc, char **argv)
{
    char *default_port = "3000";

    AppContext context;
    context.port = NULL;

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-p") == 0) && i + 1 < argc)
        {
            context.port = argv[i + 1];
        }

        if ((strcmp(argv[i], "-h") == 0))
        {
            printf("Usage: %s [-p port] [-c config_file] [-h]\n", argv[0]);
            exit(0);
        }
    }

    if (context.port == NULL)
    {
        printf("No port specified. Default is %s\n", default_port);
        context.port = default_port;
    }

    return context;
}