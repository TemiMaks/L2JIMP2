#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define API_URL "http://127.0.0.1:1234/v1/completions"
#define MODEL_NAME "qwen2.5-7b-instruct-1m"
#define MAX_INPUT 1024
#define MAX_RESPONSE 4096

// Struktura do przechowywania odpowiedzi
struct Memory {
    char *response;
    size_t size;
};

// Funkcja callback do zbierania odpowiedzi
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userdata;

    char *tmp = realloc(mem->response, mem->size + total_size + 1);
    if (!tmp) {
        printf("Błąd alokacji pamięci!\n");
        return 0;
    }

    mem->response = tmp;
    memcpy(&(mem->response[mem->size]), ptr, total_size);
    mem->size += total_size;
    mem->response[mem->size] = 0;

    return total_size;
}

// Funkcja do wysyłania zapytania i odbierania odpowiedzi
char *send_request(CURL *curl, const char *prompt) {
    struct Memory chunk = {0};
    struct curl_slist *headers = NULL;
    char json_data[MAX_INPUT + 100];

    // Przygotowanie danych JSON
    snprintf(json_data, sizeof(json_data),
            "{\"model\": \"%s\", \"prompt\": \"%s\", \"max_tokens\": 100}",
            MODEL_NAME, prompt);

    // Konfiguracja nagłówków
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Konfiguracja żądania
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    // Wysłanie żądania
    CURLcode res = curl_easy_perform(curl);

    // Sprzątanie nagłówków
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        fprintf(stderr, "Błąd cURL: %s\n", curl_easy_strerror(res));
        free(chunk.response);
        return NULL;
    }

    return chunk.response;
}

int main() {
    CURL *curl;
    char user_input[MAX_INPUT];

    // Inicjalizacja cURL
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Nie udało się zainicjalizować cURL\n");
        return 1;
    }

    // Ustawienie URL
    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    printf("Witaj w czacie z LLM! Wpisz 'exit' aby zakończyć.\n");

    // Główna pętla czatu
    while (1) {
        printf("\nTy: ");
        fgets(user_input, MAX_INPUT, stdin);

        // Usunięcie znaku nowej linii
        user_input[strcspn(user_input, "\n")] = 0;

        // Sprawdzenie warunku wyjścia
        if (strcmp(user_input, "exit") == 0) {
            break;
        }

        // Wysłanie zapytania i odebranie odpowiedzi
        char *response = send_request(curl, user_input);

        if (response) {
            printf("LLM: %s\n", response);
            free(response);
        } else {
            printf("Błąd: Nie udało się uzyskać odpowiedzi\n");
        }
    }

    // Sprzątanie
    curl_easy_cleanup(curl);

    printf("Do widzenia!\n");
    return 0;
}