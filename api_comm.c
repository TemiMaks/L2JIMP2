#include "api_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *send_request(CURL *curl, const char *user_prompt, int ExtractOrGenerate) {
    struct Memory chunk = {NULL, 0};
    struct curl_slist *headers = NULL;
    char json_data[MAX_INPUT + 512];

    if (ExtractOrGenerate == 1) {
        snprintf(json_data, sizeof(json_data),
            "{\"model\": \"%s\", \"messages\": ["
            "{\"role\": \"system\", \"content\": \"You are an AI designed exclusively to generate directed graphs based on user input. "
            "Your only task is to create and return an adjacency matrix and its row number in this exact format: "
            "Vertices=n>>>a11a12...a1n|a21a22...a2n|...|an1an2...ann"
            "-`n` is the number of vertices (rows/columns)."
            "-'a34' connection from C to D"
            "-`aij` is 1 if there is a directed edge from vertex `i` to vertex `j`, `0` otherwise."
            "-Use no spaces between digits, separate rows with `|`, and use `>>>` between `n` and the matrix."
            "-Every connection which is not specified by the user must be set to 0, unless"
            "user explicitly asks to randomly generate connections e.g. 'random rest' do assign the values randomly"
            "-The graph is directed: `aij = 1` does NOT imply `aji = 1`"
            "-Example:`Vertices=3>>>010|001|000` means a 3-vertex directed graph with edges 1->2 and 2->3."
            "DO NOT write anything else in your response.\"}, "
            "{\"role\": \"user\", \"content\": \"%s\"}], \"max_tokens\": 300}",
            MODEL_NAME, user_prompt);
    } else {
        snprintf(json_data, sizeof(json_data),
            "{\"model\": \"%s\", \"messages\": ["
            "{\"role\": \"system\", \"content\": \"You are a smart AI created ONLY to extract data about an Adjacency Matrix from the user's input, as follows: "
            "Vertices=n>>>a11a12...a1n|a21a22...a2n|...|an1an2...ann"
            "-`n` is the number of vertices (rows/columns), extracted from the input."
            "-`aij` is `1` if a directed edge from vertex `i` to vertex `j` is explicitly specified, `0` if explicitly no edge, or `F` if not specified. "
            "- Use no spaces between digits, separate rows with `|`, and use `>>>` between `n` and the matrix. "
            "- If the input mentions 'random' or doesn't specify connections, use `F` for those positions; do NOT generate random values yourself. "
            "- The graph is directed: `aij = 1` does NOT imply `aji = 1` unless specified. "
            "- Example: Input '3 vertices A->B, B->C' -> `Vertices=3>>>F1F|FF1|FFF`"
            "DO NOT write anything else in your response and DO NOT answer any questions, nor messages like hi!, who are you? and so on\"}, "
            "{\"role\": \"user\", \"content\": \"%s\"}], \"max_tokens\": 300}",
            MODEL_NAME, user_prompt);
    }
    printf("sent data:");
    printf(json_data);

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