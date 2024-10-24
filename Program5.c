//Joshua Klotzkin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_VERTICES 10000000  // Adjust based on expected graph size

// Function to perform BFS and find an augmenting path
int bfs(long long residualGraph[MAX_VERTICES][MAX_VERTICES], int source, int sink, int parent[], int numVertices) {
    int *visited = (int *)malloc(numVertices * sizeof(int));  // Dynamically allocate memory for visited
    memset(visited, 0, numVertices * sizeof(int));

    int *queue = (int *)malloc(numVertices * sizeof(int));  // Dynamically allocate memory for queue
    int front = 0, rear = 0;
    queue[rear++] = source;
    visited[source] = 1;
    parent[source] = -1;
    
    while (front < rear) {
        int u = queue[front++];
        
        for (int v = 0; v < numVertices; v++) {
            if (!visited[v] && residualGraph[u][v] > 0) {
                queue[rear++] = v;
                parent[v] = u;
                visited[v] = 1;
                
                if (v == sink) {
                    free(visited);
                    free(queue);
                    return 1;  // Path found
                }
            }
        }
    }
    
    free(visited);  // Free allocated memory after usage
    free(queue);
    return 0;  // No augmenting path
}

// Function to implement the Edmonds-Karp algorithm
int edmondsKarp(int graph[MAX_VERTICES][MAX_VERTICES], int source, int sink, int numVertices) {
    long long residualGraph[MAX_VERTICES][MAX_VERTICES];
    int u, v;

    // Initialize the residual graph with the capacities
    for (u = 0; u < numVertices; u++) {
        for (v = 0; v < numVertices; v++) {
            residualGraph[u][v] = graph[u][v];
        }
    }
    
    int *parent = (int *)malloc(numVertices * sizeof(int));  // Dynamically allocate memory for parent
    int maxFlow = 0;
    
    // Augment the flow while there is an augmenting path
    while (bfs(residualGraph, source, sink, parent, numVertices)) {
        // Find the minimum residual capacity of the edges along the path
        int pathFlow = INT_MAX;
        for (v = sink; v != source; v = parent[v]) {
            u = parent[v];
            pathFlow = (residualGraph[u][v] < pathFlow) ? residualGraph[u][v] : pathFlow;
        }

        // Update the residual capacities of the edges and reverse edges
        for (v = sink; v != source; v = parent[v]) {
            u = parent[v];
            residualGraph[u][v] -= pathFlow;
            residualGraph[v][u] += pathFlow;
        }
        
        // Add the path flow to the overall flow
        maxFlow += pathFlow;
    }
    
    free(parent);  // Free allocated memory after usage
    return maxFlow;
}

// Function to print the file or the first 10 lines if too large
void printFile(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char line[256];
    int lineCount = 0;
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
        if (++lineCount == 10) {
            printf("... (file is too large, only first 10 lines displayed)\n");
            break;
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        if (argc == 2) {
            // No source and sink provided, print the input file contents
            printFile(argv[1]);
        } else {
            printf("Usage: %s <source> <sink> < <inputfile>\n", argv[0]);
        }
        return 0;
    }

    int source = atoi(argv[1]);
    int sink = atoi(argv[2]);
    
    int V, E;
    scanf("%d %d", &V, &E);
    
    int graph[MAX_VERTICES][MAX_VERTICES] = {0};
    
    // Read the edges and capacities
    for (int i = 0; i < E; i++) {
        int u, v, capacity;
        scanf("%d %d %d", &u, &v, &capacity);
        graph[u][v] += capacity;  // Handling multiple edges between two vertices
    }
    
    // Calculate maximum flow
    int maxFlow = edmondsKarp(graph, source, sink, V);
    
    // Output the maximum flow value
    printf("Maximum Flow: %d\n", maxFlow);
    
    return 0;
}
