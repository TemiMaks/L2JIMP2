#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PARTS 3
#define NUM_TRIES 10

typedef struct {
    int u, v;
} Edge;

int countCutEdges(int num_edges, Edge* edges, int* partition);
void randomPartition(int num_nodes, int* partition, int* counts);
void optimizePpartition(int num_nodes, int num_edges, Edge* edges, int* partition, int* counts, int iterations);
void saveResultsToFile(const char* filename, int num_nodes, int* partition, int* counts, int num_edges, Edge* edges);
void processPartitions(const char* filename, int num_edges, int num_nodes, Edge* edges);
