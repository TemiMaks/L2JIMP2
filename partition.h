#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_PARTS 3
#define NUM_TRIES 10    // Number of partition attempts
#define ITERATIONS 1    // Default iterations (kept for reference, not used in optimizePartition)

typedef struct {
    int u, v;
} Edge;

extern int PARTS;
extern double THRESHOLD;

int countCutEdges(int numEdges, Edge* edges, int* partition);
void randomPartition(int numNodes, int* partition, int* counts);
void optimizePartition(int numNodes, int numEdges, Edge* edges, int* partition, int* counts);
void saveResultsToFile(const char* filename, int num_nodes, int* partition, int* counts, int num_edges, Edge* edges);
void processPartitions(const char* filename, int num_edges, int num_nodes, Edge* edges);

#ifdef _WIN32
#  ifdef BUILD_DLL
#    define API __declspec(dllexport)
#  else
#    define API __declspec(dllimport)
#  endif
#else
#  define API
#endif