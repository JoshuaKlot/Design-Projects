#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_VERTICES 1000  // Adjust based on expected graph size

// Function to perform BFS and find an augmenting path
int bfs(int residualGraph[MAX_VERTICES][MAX_VERTICES], int source, int sink, int parent[], int V) {
    int visited[V];
    memset(visited, 0, sizeof(visited));
    
    int queue[V], front = 0, rear = 0;
    queue[rear++] = source;
    visited[source] = 1;
    parent[source] = -1;
    
    while (front < rear) {
        int u = queue[front++];
        
        for (int v = 0; v < V; v++) {
            if (!visited[v] && residualGraph[u][v] > 0) {
                queue[rear++] = v;
                parent[v] = u;
                visited[v] = 1;
                
                if (v == sink) {
                    return 1;  // Path found
                }
            }
        }
    }
    
    return 0;  // No augmenting path
}

// Function to implement the Edmonds-Karp algorithm
int edmondsKarp(int graph[MAX_VERTICES][MAX_VERTICES], int source, int sink, int V) {
    int residualGraph[MAX_VERTICES][MAX_VERTICES];
    int u, v;
    
    // Initialize the residual graph with the capacities
    for (u = 0; u < V; u++) {
        for (v = 0; v < V; v++) {
            residualGraph[u][v] = graph[u][v];
        }
    }
    
    int parent[V];  // To store the augmenting path
    int maxFlow = 0;
    
    // Augment the flow while there is an augmenting path
    while (bfs(residualGraph, source, sink, parent, V)) {
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
    
    return maxFlow;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <source> <sink>\n", argv[0]);
        return 1;
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
