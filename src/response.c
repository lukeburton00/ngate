#include "../include/response.h"

#include <string.h>
#include <stdio.h>

void get_response(char response[])
{
    memset(response, 0, MAX_RESPONSE_SIZE * sizeof(char));
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello World!</h1></body></html>\r\n");
}
