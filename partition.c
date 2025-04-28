#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "partition.h"

int PARTS = DEFAULT_PARTS;
double THRESHOLD = 0.05; // Default threshold value
int PARTITION_COUNT = 0;
int PROCESS_COUNT = 0;
double TOLERANCE = 0.05;
int MOVED = 0;


int countCutEdges(int numEdges, Edge* edges, int* partition) {
    int cut = 0;
    for (int i = 0; i < numEdges; i++) {
        if (partition[edges[i].u] != partition[edges[i].v]) {
            cut++;
        }
    }
    return cut;
}

void randomPartition(int numNodes, int* partition, int* counts) {
    printf("Random partition\n");
    for (int i = 0; i < PARTS; i++) counts[i] = 0;
    int max_size = (numNodes + PARTS - 1) / PARTS;
    int tolerance = (int)(TOLERANCE * max_size);

    for (int i = 0; i < numNodes; i++) {
        int p;
        do {
            p = rand() % PARTS;
        } while (counts[p] >= max_size + tolerance);
        partition[i] = p;
        counts[p]++;
    }
}

void optimizePartition(int numNodes, int numEdges, Edge* edges, int* partition, int* counts) {
    printf("Optimizing partition\n");
    int max_iterations = 100;   //It shouldn't come to this, but let's hard stop it if so
    int max_size = (numNodes + PARTS - 1) / PARTS; // Base max size per partition
    int tolerance = (int)(TOLERANCE * max_size); // Allow extra nodes
    for (int iter = 0; iter < max_iterations; iter++) {
        int old_cut = countCutEdges(numEdges, edges, partition);
        for (int i = 0; i < numNodes; i++) {
            int current_part = partition[i];
            int best_part = current_part;
            int min_cut = old_cut;
            for (int p = 0; p < PARTS; p++) {
                if (p == current_part) continue;
                if (counts[p] + 1 > max_size + tolerance) {
                    continue; // Skip
                }
                partition[i] = p;
                counts[current_part]--;
                counts[p]++;
                int new_cut = countCutEdges(numEdges, edges, partition);
                if (new_cut < min_cut) {
                    min_cut = new_cut;
                    best_part = p;
                }
                partition[i] = current_part;
                counts[current_part]++;
                counts[p]--;
            }
            if (best_part != current_part) {
                partition[i] = best_part;
                counts[current_part]--;
                counts[best_part]++;
                printf("Node %d moved to partition %d\n", i, best_part);
                MOVED++;
            }
        }
        int new_cut = countCutEdges(numEdges, edges, partition);
        if (old_cut == 0 || (old_cut - new_cut) / (double)old_cut < THRESHOLD) {
            printf("Stopped at iteration %d\n", iter);
            break;
        }
    }
}

void saveResultsToFile(const char* filename, int numNodes, int* partition, int* counts, int numEdges, Edge* edges) {
    printf("Pisanie do pliku rozpoczete\n");
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Nie udało się otworzyć pliku do zapisu.\n");
        return;
    }
    for (int p = 0; p < PARTS; p++) {
        fprintf(file, "Partition %d (%d nodes):", p + 1, counts[p]);
        for (int i = 0; i < numNodes; i++) {
            if (partition[i] == p) {
                fprintf(file, " %d", i);
            }
        }
        fprintf(file, "\n");
    }
    fprintf(file, "Edges between partitions: %d\n", countCutEdges(numEdges, edges, partition));

    fclose(file);
}

void processPartitions(const char *filename, int numEdges, int numNodes, Edge* edges) {
    printf("Processing partition %d\n", PROCESS_COUNT);
    PROCESS_COUNT++;
    int* partition = (int*)malloc(sizeof(int) * numNodes);
    int* counts = (int*)malloc(sizeof(int) * PARTS);
    int* best_partition = (int*)malloc(sizeof(int) * numNodes);
    int* best_counts = (int*)malloc(sizeof(int) * PARTS);
    int best_cut = 1e9;

    for (int t = 0; t < NUM_TRIES; t++) {
        randomPartition(numNodes, partition, counts);
        optimizePartition(numNodes, numEdges, edges, partition, counts);

        int current_cut = countCutEdges(numEdges, edges, partition);
        if (current_cut < best_cut) {
            best_cut = current_cut;
            for (int i = 0; i < numNodes; i++) best_partition[i] = partition[i];
            for (int i = 0; i < PARTS; i++) best_counts[i] = counts[i];
        }
    }

    printf("Total nodes moved %d\n", MOVED);

    saveResultsToFile(filename, numNodes, best_partition, best_counts, numEdges, edges);

    free(partition);
    free(counts);
    free(best_partition);
    free(best_counts);
}