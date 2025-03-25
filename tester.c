#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define RESET   "\033[0m"

int validateMatrix(int expWidth, int expHeight, int expVertices) {
    char line[expWidth * 2 + 1];
    int width = 0, height = 0;

    FILE *file = fopen("output.txt", "r");
    if (!file) {
        perror("fopen failed");
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        int len = strlen(line);
        if (len == 0) {
            continue;
        }
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        if (width == 0) {
            char *ptr = line;
            while (*ptr) {
                if (*ptr == ' ') {
                    width++;
                }
                ptr++;
            }
            width++;
        }

        height++;
    }

    fclose(file);

    if (width != (expWidth + 1)|| height != (expHeight*2)) {
        printf("Expected height %d, got %d\n", expHeight, height);
        printf("Expected width %d, got %d\n", expWidth, width);
        return 1;
    }

    return 0;
}

void runTest(char *input){
    FILE *process = popen("./graph_gen 2>/dev/null | grep -E '^[0-9 ]+$' > output.txt", "w");
    if (!process) {
        perror("popen failed");
        return;
    }

    fwrite(input, 1, strlen(input), process);
    fflush(process);
    pclose(process);
    
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }
    
    int vertices = atoi(argv[1]);
    char input[100];

    //test 1
    snprintf(input, sizeof(input), "1\n%d\nrandom\na\n", vertices);
    runTest(input);
    
    if (validateMatrix(vertices, vertices, vertices) == 0) {
        printf(GREEN  "Test 1 passed\n" RESET);
    } else {
        printf(RED "Test 1 failed\n" RESET);
        
    }


    //test 2
    remove("output.txt");
    dprintf(input, sizeof(input), "1\n%d\nuser\na\nA->B, B->C, C->A", vertices);
    runTest(input);
    
    if (validateMatrix(vertices, vertices, vertices) == 0) {
        printf(GREEN "Test 2 passed\n" RESET);
    } else {
        printf(RED "Test 2 failed\n" RESET);
        
    }

    //test 3
    remove("output.txt");
    vertices = -1;
    dprintf(input, sizeof(input), "1\n%d\nuser\na\nA->B, B->C, C->A", vertices);
    runTest(input);
    
    if (validateMatrix(vertices, vertices, vertices) == 1) {
        printf(GREEN "Test 3 passed\n" RESET);
    } else {
        printf(RED "Test 3 failed\n" RESET);
        
    }

    

    //test 4
    remove("output.txt");
    vertices = 5;
    snprintf(input, sizeof(input), "2\ncreate a graph with %d vertices\na", vertices);
    runTest(input);

    if (validateMatrix(vertices, vertices, vertices) == 0) {
        printf(GREEN "Test 4 passed\n" RESET);
    } else {
        printf(RED "Test 4 failed\n" RESET);
        
    }



    return 0;
}