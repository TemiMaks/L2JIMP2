#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int parse_vertex_count(const char *input) {
    int n = atoi(input);
    if (n <= 0) {
        fprintf(stderr, "Number of vertices must be positive\n");
        return -1;
    }
    return n;
}