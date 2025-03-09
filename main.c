#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_generator.h"
#include "api_comm.h"
#include "graph_matrix.h"
#include "utils.h"

#define MAX_INPUT 512

int main() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "CURL initialization failed\n");
        return 1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    printf("Choose how to create the graph:\n");
    printf("1. Structured input (e.g., number of vertices, random or specific edges)\n");
    printf("2. Chat-based with LLM extraction (LLM extracts info, local algorithm builds matrix)\n");
    printf("3. Chat-based with LLM generation (LLM generates full matrix)\n");
    printf("Enter 1, 2, or 3: ");
    char choice[MAX_INPUT];
    fgets(choice, MAX_INPUT, stdin);
    choice[strcspn(choice, "\n")] = 0;

    AdjacencyMatrix matrix;
    if (strcmp(choice, "1") == 0) {
        printf("Enter the number of vertices: ");
        char vertex_input[MAX_INPUT];
        fgets(vertex_input, MAX_INPUT, stdin);
        vertex_input[strcspn(vertex_input, "\n")] = 0;

        int n = parse_vertex_count(vertex_input);
        if (n < 0) {
            curl_easy_cleanup(curl);
            return 1;
        }

        printf("Should the graph be random? (yes/no): ");
        char random_choice[MAX_INPUT];
        fgets(random_choice, MAX_INPUT, stdin);
        random_choice[strcspn(random_choice, "\n")] = 0;

        if (strcasecmp(random_choice, "yes") == 0) {
            matrix = generate_random_graph(n);
        } else if (strcasecmp(random_choice, "no") == 0) {
            printf("Enter edges (e.g., 'A->B, B->C'), or leave empty for no edges: ");
            char edge_input[MAX_INPUT];
            fgets(edge_input, MAX_INPUT, stdin);
            edge_input[strcspn(edge_input, "\n")] = 0;

            matrix = generate_user_defined_graph(n, edge_input);
        } else {
            fprintf(stderr, "Invalid choice\n");
            curl_easy_cleanup(curl);
            return 1;
        }
    } else if (strcmp(choice, "2") == 0) {
        printf("Enter your request (e.g., 'Create a graph with 5 vertices, A->B, B->C'):\n");
        char user_input[MAX_INPUT];
        fgets(user_input, MAX_INPUT, stdin);
        user_input[strcspn(user_input, "\n")] = 0;

        int n = parse_vertex_count(user_input);
        if (n < 0) {
            fprintf(stderr, "Could not determine number of vertices\n");
            curl_easy_cleanup(curl);
            return 1;
        }

        char *response = send_request(curl, user_input, 0); // Extract mode
        if (response) {
            printf("API Response:\n%s\n", response);
            matrix = create_matrix_from_extracted(response, n);
            free(response);
        } else {
            fprintf(stderr, "API communication error\n");
            curl_easy_cleanup(curl);
            return 1;
        }
    } else if (strcmp(choice, "3") == 0) {
        printf("Enter your request (e.g., 'Create a graph with 5 vertices' or '5 A->B, B->C'):\n");
        char user_input[MAX_INPUT];
        fgets(user_input, MAX_INPUT, stdin);
        user_input[strcspn(user_input, "\n")] = 0;

        char *response = send_request(curl, user_input, 1); // Generate mode
        if (response) {
            printf("API Response:\n%s\n", response);
            matrix = parse_adjacency_matrix(response);
            free(response);
        } else {
            fprintf(stderr, "API communication error\n");
            curl_easy_cleanup(curl);
            return 1;
        }
    } else {
        fprintf(stderr, "Invalid option. Please enter 1, 2, or 3.\n");
        curl_easy_cleanup(curl);
        return 1;
    }

    if (matrix.matrix == NULL) {
        fprintf(stderr, "Failed to create graph\n");
        curl_easy_cleanup(curl);
        return 1;
    }

    printf("n=%d\n", matrix.n);
    print_adjacency_matrix(&matrix);
    free_adjacency_matrix(&matrix);

    curl_easy_cleanup(curl);
    return 0;
}