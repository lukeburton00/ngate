#pragma once

struct config
{
    char port[6];
    char proxy_port[6];
    char *filepath;
};

struct config create_config(int argc, char **argv);
int is_valid_port(const char *port);
