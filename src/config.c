#include "../include/config.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct config_option
{
    char key[128];
    char value[128];
};

static const char *usage = ("Usage: ngate (args)\n"
       "-f [path] - define path to configuration file\n"
       "-h - display options\n");

struct config create_config(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("%s", usage);
        exit(1);
    }

    struct config config = {.filepath = NULL};

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-f") == 0) && i + 1 < argc)
        {
            config.filepath = argv[i + 1];
            i++;
        }

        else if ((strcmp(argv[i], "-h") == 0))
        {
            printf("%s", usage);
            exit(0);
        }

        else
        {
            printf("Ignoring unrecognized parameter %s\n", argv[i]);
        }
    }

    if (!config.filepath)
    {
        printf("No configuration file provided. %s", usage);
        exit(1);
    }

    FILE *config_file = fopen(config.filepath, "r");
    if (!config_file)
    {
        fprintf(stderr, "Failed to open config file\n");
        exit(1);
    }

    struct config_option *option = calloc(1, sizeof(struct config_option));
    if (!option)
    {
        fprintf(stderr, "Calloc error");
        exit(1);
    }

    while(fscanf(config_file, "%s = %s", option->key, option->value) == 2)
    {
        if (strcmp(option->key, "port") == 0)
        {
            strncpy(config.port, option->value, sizeof(config.port) - 1);
            config.port[sizeof(config.port) - 1] = '\0';
            continue;
        }

        if (strcmp(option->key, "proxy_port") == 0)
        {
            strncpy(config.proxy_port, option->value, sizeof(config.proxy_port) - 1);
            config.proxy_port[sizeof(config.proxy_port) - 1] = '\0';
            continue;
        }

        if (strcmp(option->key, "#") == 0)
        {
            while (fgetc(config_file) != '\n'){}
            continue;
        }
    }

    fclose(config_file);
    free(option);

    if (!is_valid_port(config.port))
    {
        fprintf(stderr, "Parameter port was not defined or is invalid\n");
        exit(1);
    }

    if (!is_valid_port(config.proxy_port))
    {
        fprintf(stderr, "Parameter proxy_port was not defined or is invalid\n");
        exit(1);
    }

    return config;
}

int is_valid_port(const char *port)
{
    if (port == NULL)
    {
        return 0;
    }

    for (int i = 0; port[i] != '\0'; i++)
    {
        if (!isdigit(port[i]))
        {
            return 0;
        }
    }

    int port_num = atoi(port);
    if (port_num >= 0 && port_num <= 65535)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
