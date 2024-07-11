#pragma once

struct config
{
    char *port;
};

struct config create_config(int argc, char **argv);
