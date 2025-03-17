#include "graph_matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AdjacencyMatrix parse_adjacency_matrix(const char *json_response) {
    AdjacencyMatrix matrix = {NULL, 0};

    // Find "content" field
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

    // Extract content string
    size_t content_length = content_end - content_start;
    char *content = malloc(content_length + 1);
    if (!content) {
        fprintf(stderr, "Błąd alokacji pamięci\n");
        return matrix;
    }
    strncpy(content, content_start, content_length);
    content[content_length] = '\0';

    // Extract number of vertices
    const char *vertices_start = strstr(content, "Vertices=");
    if (!vertices_start) {
        fprintf(stderr, "Nie znaleziono 'Vertices=' w odpowiedzi\n");
        free(content);
        return matrix;
    }
    vertices_start += strlen("Vertices=");
    int num_rows = atoi(vertices_start);
    if (num_rows <= 0) {
        fprintf(stderr, "Nieprawidłowa liczba wierzchołków\n");
        free(content);
        return matrix;
    }

    // Find matrix start
    const char *matrix_start = strstr(content, ">>>");
    if (!matrix_start) {
        fprintf(stderr, "Nie znaleziono '>>>' w odpowiedzi\n");
        free(content);
        return matrix;
    }
    matrix_start += strlen(">>>");

    // Allocate matrix
    matrix.n = num_rows;
    matrix.matrix = malloc(num_rows * sizeof(int *));
    if (!matrix.matrix) {
        fprintf(stderr, "Błąd alokacji pamięci dla macierzy\n");
        free(content);
        return matrix;
    }
    for (int i = 0; i < num_rows; i++) {
        matrix.matrix[i] = malloc(num_rows * sizeof(int));
        if (!matrix.matrix[i]) {
            fprintf(stderr, "Błąd alokacji pamięci dla wiersza\n");
            while (i > 0) free(matrix.matrix[--i]);
            free(matrix.matrix);
            free(content);
            matrix.matrix = NULL;
            return matrix;
        }
        memset(matrix.matrix[i], 0, num_rows * sizeof(int));
    }

    // Parse matrix with '|' as delimiter
    char *matrix_copy = strdup(matrix_start);
    if (!matrix_copy) {
        fprintf(stderr, "Błąd alokacji pamięci dla kopii macierzy\n");
        free_adjacency_matrix(&matrix);
        free(content);
        return matrix;
    }

    int i = 0;
    char *line = strtok(matrix_copy, "|");
    while (line != NULL && i < num_rows) {
        int j = 0;
        char *num = line;
        while (*num != '\0' && j < num_rows) {
            if (*num == 'X') {
                fprintf(stderr, "Błąd: Model AI zwrócił macierz z 'X', co oznacza niemożliwe połączenia w wiadomości użytkownika\n");
                free(matrix_copy);
                free_adjacency_matrix(&matrix);
                free(content);
                matrix.matrix = NULL;
                matrix.n = 0;
                return matrix;
            }
            else if (*num >= '0' && *num <= '1') {
                matrix.matrix[i][j] = (*num - '0');
            } else {
                fprintf(stderr, "Nieprawidłowy znak w wierszu %d: %c\n", i, *num);
            }
            j++;
            num++;
        }
        if (j != num_rows) {
            fprintf(stderr, "Niewłaściwa liczba elementów w wierszu %d (oczekiwano %d, znaleziono %d)\n", i, num_rows, j);
        }
        i++;
        line = strtok(NULL, "|");
    }
    if (i != num_rows) {
        fprintf(stderr, "Niezgodna liczba wierszy w danych (oczekiwano %d, znaleziono %d)\n", num_rows, i);
    }

    free(matrix_copy);
    free(content);
    return matrix;
}

void print_adjacency_matrix(const AdjacencyMatrix *matrix) {
    for (int i = 0; i < matrix->n; i++) {
        for (int j = 0; j < matrix->n; j++) {
            printf("%d ", matrix->matrix[i][j]);
        }
        printf("\n");
    }
}

void free_adjacency_matrix(AdjacencyMatrix *matrix) {
    for (int i = 0; i < matrix->n; i++) {
        free(matrix->matrix[i]);
    }
    free(matrix->matrix);
}