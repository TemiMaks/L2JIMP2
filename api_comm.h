#ifndef API_COMM_H
#define API_COMM_H

#include <curl/curl.h>

// Deklaracja struktury do przechowywania odpowiedzi
struct Memory {
    char *response;
    size_t size;
};

// Funkcje związane z komunikacją z API
char *send_request(CURL *curl, const char *user_prompt);
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata);

#endif // API_COMM_H
