#include <stdio.h>
#include <stdlib.h>
#include "csrrg.h"
#include "partition.h"

extern double TOLERANCE;

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s <filename> <number_of_partitions> <threshold> <tolerance>\n", argv[0]);
        return 1;
    }

    PARTS = atoi(argv[2]);
    if (PARTS < 2 || PARTS > 100) {
        printf("Invalid number of partitions. Must be between 2 and 100.\n");
        return 1;
    }
    double threshold = atof(argv[3]);
    if (threshold < 0 || threshold > 1) {
        if (threshold == 0) fprintf(stderr, "Selected threshold will run 100 times even if improvement is 0!");
        printf("Invalid threshold. Must be between 0 and 1.\n");
        return 1;
    }
    THRESHOLD = threshold;

    double tolerance = atof(argv[4]);
    if (tolerance < 0 || tolerance > 1) {
        printf("Invalid tolerance. Must be between 0 and 1.\n");
        return 1;
    }
    TOLERANCE = tolerance;

    return processCsrrgFile(argv[1]);
}