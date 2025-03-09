#include "api_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODEL_NAME "qwen2.5-7b-instruct-1m"
#define MAX_INPUT 1024

// Funkcja zapisująca odpowiedź do struktury
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userdata;

    char *tmp = realloc(mem->response, mem->size + total_size + 1);
    if (!tmp) {
        return 0;
    }

    mem->response = tmp;
    memcpy(&(mem->response[mem->size]), ptr, total_size);
    mem->size += total_size;
    mem->response[mem->size] = '\0';

    return total_size;
}

// Funkcja wysyłająca zapytanie do API i pobierająca odpowiedź
char *send_request(CURL *curl, const char *user_prompt) {
    struct Memory chunk = {NULL, 0};
    struct curl_slist *headers = NULL;
    char json_data[MAX_INPUT + 512];

    snprintf(json_data, sizeof(json_data),
             "{\"model\": \"%s\", \"messages\": ["
             "{\"role\": \"system\", \"content\": \"You are an AI designed exclusively to generate directed graphs based on user input. I'd like you to generate random connections if none are specified "
             "Your only task is to create and return an adjacency matrix, DO NOT write anything else in your response, as follows:"
             "010\\n100\\n000"
             "Do not make spaces between the digits but add new line characters \\n"
             "is an example which means: the vertice 1 is connected to vertice 2 (first row and second column is 1), the 3rd is not connected anywhere (hence every number in the 3rd row is zero)"
             "notice that if the 1 vertice is connected to 2 then the first row second column must be 1 AND the second row first column MUST be one\"}, "
             "{\"role\": \"user\", \"content\": \"%s\"}], \"max_tokens\": 300}",
             MODEL_NAME, user_prompt);

    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        free(chunk.response);
        return NULL;
    }

    return chunk.response;
}
