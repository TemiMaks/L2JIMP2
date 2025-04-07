#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "partition.h"

int countCutEdges(int num_edges, Edge* edges, int* partition) {
    int cut = 0;
    for (int i = 0; i < num_edges; i++) {
        if (partition[edges[i].u] != partition[edges[i].v]) {
            cut++;
        }
    }
    return cut;
}

void randomPartition(int num_nodes, int* partition, int* counts) {
    for (int i = 0; i < PARTS; i++) counts[i] = 0;
    for (int i = 0; i < num_nodes; i++) {
        int p;
        do {
            p = rand() % PARTS;
        } while (counts[p] >= (num_nodes + PARTS - 1) / PARTS);
        partition[i] = p;
        counts[p]++;
    }
}

void optimizePartition(int num_nodes, int num_edges, Edge* edges, int* partition, int* counts, int iterations) {
    for (int iter = 0; iter < iterations; iter++) {
        for (int i = 0; i < num_nodes; i++) {
            int current_part = partition[i];
            int best_part = current_part;
            int min_cut = countCutEdges(num_edges, edges, partition);

            for (int p = 0; p < PARTS; p++) {
                if (p == current_part) continue;
                if (counts[p] + 1 > (num_nodes + PARTS - 1) / PARTS) continue;

                partition[i] = p;
                counts[current_part]--;
                counts[p]++;
                int new_cut = countCutEdges(num_edges, edges, partition);

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
            }
        }
    }
}

void saveResultsToFile(const char* filename, int num_nodes, int* partition, int* counts, int num_edges, Edge* edges) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Nie udało się otworzyć pliku do zapisu.\n");
	return;
    }
    for (int p = 0; p < PARTS; p++) {
        fprintf(file, "Partition %d (%d nodes):", p + 1, counts[p]);
        for (int i = 0; i < num_nodes; i++) {
            if (partition[i] == p) {
                fprintf(file, " %d", i);
            }
        }
        fprintf(file, "\n");
    }
    fprintf(file, "Edges between partitions: %d\n", countCutEdges(num_edges, edges, partition));

    fclose(file);
}
 
void processPartitions(const char *filename, int num_edges, int num_nodes, Edge* edges) {
    int* partition = (int*)malloc(sizeof(int) * num_nodes);
    int* counts = (int*)malloc(sizeof(int) * PARTS);
    int* best_partition = (int*)malloc(sizeof(int) * num_nodes);
    int* best_counts = (int*)malloc(sizeof(int) * PARTS);
    int best_cut = 1e9;

    for (int t = 0; t < NUM_TRIES; t++) {
        randomPartition(num_nodes, partition, counts);
        optimizePartition(num_nodes, num_edges, edges, partition, counts, 100);

        int current_cut = countCutEdges(num_edges, edges, partition);
        if (current_cut < best_cut) {
            best_cut = current_cut;
            for (int i = 0; i < num_nodes; i++) best_partition[i] = partition[i];
            for (int i = 0; i < PARTS; i++) best_counts[i] = counts[i];
        }
    }

    saveResultsToFile(filename, num_nodes, best_partition, best_counts, num_edges, edges);

    free(partition);
    free(counts);
    free(best_partition);
    free(best_counts);
}
