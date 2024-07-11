#pragma once

#define MAX_REQUEST_SIZE 1000000

struct session;

int parse_request(struct session *session);
