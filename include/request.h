#pragma once

#define MAX_REQUEST_SIZE 1000000

typedef struct Session Session;

int parse_request(Session *session);