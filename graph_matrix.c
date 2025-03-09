#include "graph_matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funkcja wczytująca macierz sąsiedztwa z odpowiedzi JSON
AdjacencyMatrix parse_adjacency_matrix(const char *json_response) {
    AdjacencyMatrix matrix = {NULL, 0};

    const char *content_start = strstr(json_response, "\"content\": \"");
    if (!content_start) {
        fprintf(stderr, "Nie znaleziono odpowiedzi w JSON\n");
        return matrix;
    }

    content_start += strlen("\"content\": \"");
    const char *content_end = strchr(content_start, '"');
    if (!content_end) {
        fprintf(stderr, "Błąd w formacie odpowiedzi JSON\n");
        return matrix;
    }

    size_t content_length = content_end - content_start;
    char *content = malloc(content_length + 1);
    if (!content) {
        fprintf(stderr, "Błąd alokacji pamięci\n");
        return matrix;
    }

    strncpy(content, content_start, content_length);
    content[content_length] = '\0';

    // Zliczanie liczby wierszy
    int num_rows = 0;
    char *line = strtok(content, "\n");
    while (line != NULL) {
        num_rows++;
        line = strtok(NULL, "\n");
    }

    matrix.n = num_rows;

    matrix.matrix = malloc(num_rows * sizeof(int *));
    for (int i = 0; i < num_rows; i++) {
        matrix.matrix[i] = malloc(num_rows * sizeof(int));
    }

    // Wypełnianie macierzy
    int i = 0;
    line = strtok(content, "\n");
    while (line != NULL) {
        int j = 0;
        char *num = strtok(line, " ");
        while (num != NULL) {
            matrix.matrix[i][j] = atoi(num);
            j++;
            num = strtok(NULL, " ");
        }
        i++;
        line = strtok(NULL, "\n");
    }

    free(content);
    return matrix;
}

// Funkcja do wypisania macierzy sąsiedztwa
void print_adjacency_matrix(const AdjacencyMatrix *matrix) {
    for (int i = 0; i < matrix->n; i++) {
        for (int j = 0; j < matrix->n; j++) {
            printf("%d ", matrix->matrix[i][j]);
        }
        printf("\n");
    }
}

// Funkcja czyszcząca pamięć zajmowaną przez macierz
void free_adjacency_matrix(AdjacencyMatrix *matrix) {
    for (int i = 0; i < matrix->n; i++) {
        free(matrix->matrix[i]);
    }
    free(matrix->matrix);
}
