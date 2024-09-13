//Joshua Klotzkin

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX // Infinite distance

// Structure to represent a directed edge
struct Edge {
    int to;
    int length;
};

// Function to find the vertex with the smallest distance
int find_min_distance(int dist[], int visited[], int num_vertices) {
    int min = INF, min_index;

    for (int v = 0; v < num_vertices; v++) {
        if (!visited[v] && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// Dijkstra's algorithm to find the shortest path from start to end
int dijkstra(int start, int end, struct Edge** graph, int* num_edges, int num_vertices) {
    int* dist = (int*)malloc(num_vertices * sizeof(int));   // Distance array
    int* visited = (int*)malloc(num_vertices * sizeof(int)); // Visited array

    for (int i = 0; i < num_vertices; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }
    dist[start] = 0;

    for (int i = 0; i < num_vertices - 1; i++) {
        int u = find_min_distance(dist, visited, num_vertices);
        visited[u] = 1;

        // Update distance for adjacent vertices
        for (int j = 0; j < num_edges[u]; j++) {
            int v = graph[u][j].to;
            int weight = graph[u][j].length;
            if (!visited[v] && dist[u] != INF && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
            }
        }
    }

    int result = dist[end];
    free(dist);
    free(visited);
    return (result == INF) ? -1 : result; // Return -1 if unreachable
}

int main() {
    int num_vertices, num_edges;

    // Read number of vertices and edges
    if (scanf("%d %d", &num_vertices, &num_edges) != 2) {
        fprintf(stderr, "Error reading number of vertices and edges.\n");
        return 1;
    }

    // Allocate memory for the graph
    struct Edge** graph = (struct Edge**)malloc(num_vertices * sizeof(struct Edge*));
    int* num_edges_per_vertex = (int*)malloc(num_vertices * sizeof(int));
    for (int i = 0; i < num_vertices; i++) {
        graph[i] = (struct Edge*)malloc(num_edges * sizeof(struct Edge)); // Allocate space for edges
        num_edges_per_vertex[i] = 0; // Initialize count of edges per vertex
    }

    // Read edges
    for (int i = 0; i < num_edges; i++) {
        int u, v, length;
        if (scanf("%d %d %d", &u, &v, &length) != 3) {
            fprintf(stderr, "Error reading edge.\n");
            return 1;
        }
        graph[u][num_edges_per_vertex[u]].to = v;
        graph[u][num_edges_per_vertex[u]].length = length;
        num_edges_per_vertex[u]++;
    }

    // Read start and end vertices from the command line
    if (scanf("%d %d", &start_vertex, &end_vertex) != 2) {
        fprintf(stderr, "Error reading start and end vertices.\n");
        return 1;
    }

    // Find the shortest path from start_vertex to end_vertex
    int shortest_path = dijkstra(start_vertex, end_vertex, graph, num_edges_per_vertex, num_vertices);

    if (shortest_path == -1) {
        printf("unconnected\n");
    } else {
        printf("Shortest path distance: %d\n", shortest_path);
    }

    // Free allocated memory
    for (int i = 0; i < num_vertices; i++) {
        free(graph[i]);
    }
    free(graph);
    free(num_edges_per_vertex);

    return 0;
}
