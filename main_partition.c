#include <stdio.h>
#include <stdlib.h>
#include "csrrg.h"
#include "partition.h"

extern void processPartitions(const char *filename, int num_edges, int num_nodes, Edge *edges);

extern int PARTS;
extern double THRESHOLD;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <filename> <number_of_partitions> <threshold>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    PARTS = atoi(argv[2]);
    if (PARTS < 2 || PARTS > 100) {
        printf("Invalid number of partitions. Must be between 2 and 100.\n");
        return 1;
    }
    double threshold = atof(argv[3]);
    if (threshold <= 0 || threshold >= 1) {
        printf("Invalid threshold. Must be between 0 and 1.\n");
        return 1;
    }
    THRESHOLD = threshold;

    return processCsrrgFile(argv[1]);
}