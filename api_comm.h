#ifndef API_COMM_H
#define API_COMM_H

#include <curl/curl.h>

#define API_URL "http://127.0.0.1:1234/v1/chat/completions"
#define MODEL_NAME "llama-3.2-1b-instruct"
#define MAX_INPUT 512

struct Memory {
    char *response;
    size_t size;
};

char *send_request(CURL *curl, const char *user_prompt, int mode);

#endif