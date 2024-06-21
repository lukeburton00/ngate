#pragma once

#define MAX_RESPONSE_SIZE 1000000

typedef struct Session Session;
typedef struct AppContext AppContext;

void get_response(char response[], AppContext *context, Session *session);