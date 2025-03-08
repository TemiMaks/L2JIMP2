#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define API_URL "http://127.0.0.1:1234/v1/chat/completions"
#define MODEL_NAME "qwen2.5-7b-instruct-1m"
#define MAX_INPUT 1024
#define MAX_RESPONSE 4096

// Structure to store the response
struct Memory {
    char *response;
    size_t size;
};

// Callback function to collect the response
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
    mem->response[mem->size] = '\0';

    return total_size;
}

// Function to extract the content field from the JSON response
char *extract_content(const char *response) {
    // Debug: Print first 50 characters of response to check for invisible characters
    printf("First 50 chars of response: '");
    for (int i = 0; i < 50 && response[i] != '\0'; i++) {
        if (response[i] == '\n') printf("\\n");
        else if (response[i] == '\t') printf("\\t");
        else printf("%c", response[i]);
    }
    printf("'\n");

    // Search for "choices" key
    const char *choices_start = strstr(response, "\"choices\":");
    if (!choices_start) {
        return strdup("Error: Could not find choices key");
    }

    // Move to the start of the array and look for the opening bracket
    choices_start += strlen("\"choices\":");
    while (*choices_start == ' ' || *choices_start == '\n' || *choices_start == '\t') {
        choices_start++; // Skip whitespace
    }
    if (*choices_start != '[') {
        return strdup("Error: Choices array does not start with [");
    }

    // Move past the opening bracket
    choices_start++;
    while (*choices_start == ' ' || *choices_start == '\n' || *choices_start == '\t') {
        choices_start++; // Skip whitespace
    }

    // Find the first object in the array
    if (*choices_start != '{') {
        return strdup("Error: Choices array does not contain an object");
    }

    // Debug: Print the substring we're searching in
    printf("Choices object start: '");
    for (int i = 0; i < 50 && choices_start[i] != '\0'; i++) {
        if (choices_start[i] == '\n') printf("\\n");
        else if (choices_start[i] == '\t') printf("\\t");
        else printf("%c", choices_start[i]);
    }
    printf("'\n");

    // Search for "message" key (without the { to handle whitespace)
    const char *message_start = strstr(choices_start, "\"message\":");
    if (!message_start) {
        return strdup("Error: Could not find message key");
    }

    // Move past "message": and look for the opening brace
    message_start += strlen("\"message\":");
    while (*message_start == ' ' || *message_start == '\n' || *message_start == '\t') {
        message_start++; // Skip whitespace
    }
    if (*message_start != '{') {
        return strdup("Error: Message object does not start with {");
    }

    const char *content_start = strstr(message_start, "\"content\":\"");
    if (!content_start) {
        return strdup("Error: Could not find content field");
    }

    content_start += strlen("\"content\":\""); // Move past the "content":" part
    const char *content_end = strstr(content_start, "\"");
    if (!content_end) {
        return strdup("Error: Could not find end of content");
    }

    size_t content_len = content_end - content_start;
    char *content = (char *)malloc(content_len + 1);
    if (!content) {
        return strdup("Error: Memory allocation failed");
    }
    strncpy(content, content_start, content_len);
    content[content_len] = '\0';

    return content;
}

// Function to send a request and receive a response
char *send_request(CURL *curl, const char *system_prompt, const char *user_prompt) {
    struct Memory chunk = {0};
    struct curl_slist *headers = NULL;
    char json_data[MAX_INPUT + 2048]; // Increased size for longer prompt

    // Prepare JSON data for chat completions
    snprintf(json_data, sizeof(json_data),
            "{\"model\": \"%s\", \"messages\": ["
            "{\"role\": \"system\", \"content\": \"%s\"}, "
            "{\"role\": \"user\", \"content\": \"%s\"}], "
            "\"max_tokens\": 150}",
            MODEL_NAME, system_prompt, user_prompt);

    // Set up headers
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Configure the request
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    // Send the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up headers
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        fprintf(stderr, "Błąd cURL: %s\n", curl_easy_strerror(res));
        free(chunk.response);
        return NULL;
    }

    // Debug: Print raw response
    printf("Raw Response: %s\n", chunk.response);

    // Extract the content field from the response
    char *content = extract_content(chunk.response);
    free(chunk.response); // Free the raw response
    return content;
}

int main() {
    CURL *curl;
    char user_input[MAX_INPUT];

    // Initialize cURL
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Nie udało się zainicjalizować cURL\n");
        return 1;
    }

    // Set the URL
    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // Initial system prompt to set up the AI's role
    const char *initial_system_prompt = "You are an AI designed exclusively to generate directed graphs based on user input. Your only task is to create and describe directed graphs; if a request is not related to graph generation, respond with: 'I was trained solely for graph generation and I'm not able to answer any other question.' When a user requests a graph, follow their instructions precisely, including the number of vertices, edges, and directions. If 'two-way' connections are specified, represent them as two directed edges (e.g., A->B and B->A). If details are unspecified, creatively determine a logical graph structure with vertices, connections, and directions. Output the graph as a JSON object with 'vertices' (a list of vertex names) and 'edges' (a list of directed edges, e.g., A->B). Example output: vertices: A, B, C; edges: A->B, B->A, B->C, C->B, C->A, A->C.";

    printf("Witaj w generatorze grafów skierowanych! Wpisz 'exit' aby zakończyć.\n");
    printf("Możesz podać konkretne instrukcje, np. 'Stwórz graf z 3 wierzchołkami: A->B, B->C', lub pozostawić to AI.\n");

    // Main chat loop (starts with user input)
    while (1) {
        printf("\nTy: ");
        fgets(user_input, MAX_INPUT, stdin);

        // Remove newline character
        user_input[strcspn(user_input, "\n")] = 0;

        // Check for exit condition
        if (strcmp(user_input, "exit") == 0) {
            break;
        }

        // Send request and receive response
        char *response = send_request(curl, initial_system_prompt, user_input);

        if (response) {
            printf("Graf: %s\n", response);
            free(response);
        } else {
            printf("Błąd: Nie udało się uzyskać odpowiedzi\n");
        }
    }

    // Cleanup
    curl_easy_cleanup(curl);

    printf("Do widzenia!\n");
    return 0;
}