#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "api_comm.h"
#include "graph_matrix.h"

#define API_URL "http://127.0.0.1:1234/v1/chat/completions"
#define MAX_INPUT  512
int main() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    printf("Podaj liczbę wierzchołków (opcjonalnie krawędzie np. '5 A->B, B->C'):\n");
    char user_input[MAX_INPUT];
    fgets(user_input, MAX_INPUT, stdin);
    user_input[strcspn(user_input, "\n")] = 0;

    char *response = send_request(curl, user_input);
    if (response) {
        printf("Odpowiedź z API:\n%s\n", response);
        AdjacencyMatrix matrix = parse_adjacency_matrix(response);
        printf("n=%d\n", matrix.n);
        printf("1\n");
        print_adjacency_matrix(&matrix);
        printf("2\n");
        free_adjacency_matrix(&matrix);
        free(response);
    }

    curl_easy_cleanup(curl);
    return 0;
}
