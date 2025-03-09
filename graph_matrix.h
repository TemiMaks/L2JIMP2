#ifndef GRAPH_MATRIX_H
#define GRAPH_MATRIX_H

// Struktura reprezentująca macierz sąsiedztwa
typedef struct AdjacencyMatrix {
    int **matrix;  // Dwuwymiarowa tablica reprezentująca macierz
    int n;         // Liczba wierzchołków
} AdjacencyMatrix;

// Funkcje do przetwarzania macierzy
AdjacencyMatrix parse_adjacency_matrix(const char *json_response);
void print_adjacency_matrix(const AdjacencyMatrix *matrix);
void free_adjacency_matrix(AdjacencyMatrix *matrix);

#endif // GRAPH_MATRIX_H
