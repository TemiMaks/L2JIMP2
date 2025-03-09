#include "graph_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AdjacencyMatrix generate_random_graph(int n) {
    AdjacencyMatrix matrix = {NULL, n};
    matrix.matrix = malloc(n * sizeof(int *));
    if (!matrix.matrix) {
        fprintf(stderr, "Memory allocation error\n");
        return matrix;
    }
    for (int i = 0; i < n; i++) {
        matrix.matrix[i] = malloc(n * sizeof(int));
        if (!matrix.matrix[i]) {
            fprintf(stderr, "Memory allocation error for row\n");
            while (i > 0) free(matrix.matrix[--i]);
            free(matrix.matrix);
            matrix.matrix = NULL;
            return matrix;
        }
        for (int j = 0; j < n; j++) {
            matrix.matrix[i][j] = rand() % 2; // Random 0 or 1
        }
    }
    return matrix;
}

AdjacencyMatrix generate_user_defined_graph(int n, const char *edges) {
    AdjacencyMatrix matrix = {NULL, n};
    matrix.matrix = malloc(n * sizeof(int *));
    if (!matrix.matrix) {
        fprintf(stderr, "Memory allocation error\n");
        return matrix;
    }
    for (int i = 0; i < n; i++) {
        matrix.matrix[i] = calloc(n, sizeof(int)); // Zero-initialized
        if (!matrix.matrix[i]) {
            fprintf(stderr, "Memory allocation error for row\n");
            while (i > 0) free(matrix.matrix[--i]);
            free(matrix.matrix);
            matrix.matrix = NULL;
            return matrix;
        }
    }

    if (edges && edges[0] != '\0') {
        printf("Edge parsing: Yet to be implemented\n");
        // TODO: Parse edges like "A->B, B->C" and fill matrix
    }

    return matrix;
}

AdjacencyMatrix create_matrix_from_extracted(const char *response, int n) {
    AdjacencyMatrix matrix = {NULL, n};
    matrix.matrix = malloc(n * sizeof(int *));
    if (!matrix.matrix) {
        fprintf(stderr, "Memory allocation error\n");
        return matrix;
    }
    for (int i = 0; i < n; i++) {
        matrix.matrix[i] = calloc(n, sizeof(int)); // Zero-initialized
        if (!matrix.matrix[i]) {
            fprintf(stderr, "Memory allocation error for row\n");
            while (i > 0) free(matrix.matrix[--i]);
            free(matrix.matrix);
            matrix.matrix = NULL;
            return matrix;
        }
    }

    const char *matrix_start = strstr(response, ">>>");
    if (!matrix_start) {
        fprintf(stderr, "Invalid extracted format: '>>>' not found\n");
        free_adjacency_matrix(&matrix);
        return matrix;
    }
    matrix_start += strlen(">>>");

    char *matrix_copy = strdup(matrix_start);
    if (!matrix_copy) {
        fprintf(stderr, "Memory allocation error for matrix copy\n");
        free_adjacency_matrix(&matrix);
        return matrix;
    }

    int i = 0;
    char *line = strtok(matrix_copy, "|");
    while (line != NULL && i < n) {
        int j = 0;
        char *num = line;
        while (*num != '\0' && j < n) {
            if (*num >= '0' && *num <= '1') {
                matrix.matrix[i][j] = (*num - '0');
            }
            j++;
            num++;
        }
        i++;
        line = strtok(NULL, "|");
    }
    free(matrix_copy);

    if (i != n) {
        fprintf(stderr, "Mismatch in number of rows (expected %d, found %d)\n", n, i);
        free_adjacency_matrix(&matrix);
        matrix.matrix = NULL;
    }

    return matrix;
}