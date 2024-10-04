//Joshua Klotzkin
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX // This is infinity incase the nodes dont connect

// The Edge object contains the node it points to and the length of the edge
struct Edge {
    int to;      
    int length;  
};

// A MinHeapNode object contains a vertex and its currently known shortest distance from the source vertex
struct MinHeapNode {
    int v;    
    int dist;
};

// A MinHeap object contains a MinNodeHeap array and manages their positions for efficient access
struct MinHeap {
    int size;      
    int capacity;  
    int *pos;      
    struct MinHeapNode **array; 
};

//Function to help create a new MinHeapNode for a given vertex and its distance
struct MinHeapNode* newMinHeapNode(int v, int dist) {
    struct MinHeapNode* minHeapNode = (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

//Function to help create a MinHeap and set up its capacity
struct MinHeap* createMinHeap(int capacity) {
    struct MinHeap* minHeap = (struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

//Function to swap two nodes in the min heap
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Function to maintain the min-heap property by "heapifying" a subtree with root at index `idx`
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    // If the left child exists and is smaller than the current smallest make the smallest the left
    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;

    // If the right child exists and is smaller than the current smallest make the smallest the right
    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    // If the smallest element is not the root, swap with the smallest and heapify the affected subtree
    if (smallest != idx) {
        struct MinHeapNode* smallestNode = minHeap->array[smallest];
        struct MinHeapNode* idxNode = minHeap->array[idx];

        // Update the position of the nodes in the heap
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        // Swap the two nodes
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        // Recursively heapify the affected subtree
        minHeapify(minHeap, smallest);
    }
}

// Function to check if the heap is empty
int isEmpty(struct MinHeap* minHeap) {
    return minHeap->size == 0;
}

//Function to extract the root from the min heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return NULL;

    // The root of the heap is the minimum element
    struct MinHeapNode* root = minHeap->array[0];

    // Replace the root with the last element in the heap
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // Update the position of the last node
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;

    // Reduce the heap size and heapify from the root to maintain the min-heap property
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// Function to decrease the distance value of a given vertex `v` in the heap
void decreaseKey(struct MinHeap* minHeap, int v, int dist) {
    
    int i = minHeap->pos[v];

    
    minHeap->array[i]->dist = dist;

    // Traverse up the tree and fix the heap property if necessary
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist) {

        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        // Move to the parent index
        i = (i - 1) / 2;
    }
}

// Function to check if a given vertex `v` is in the min heap or not
int isInMinHeap(struct MinHeap *minHeap, int v) {
   return minHeap->pos[v] < minHeap->size;
}

// Prim's algorithm to find the Minimum Spanning Tree (MST) using a MinHeap
int primMST(struct Edge** graph, int* num_edges, int num_vertices) {
    int* parent = (int*)malloc(num_vertices * sizeof(int)); // Stores the parent

    int* key = (int*)malloc(num_vertices * sizeof(int)); // Stores the minimum edge weight to add the vertex to the MST.

    struct MinHeap* minHeap = createMinHeap(num_vertices); // Priority queue to efficiently find the next vertex to add to the MST.

    // Initialize the heap and key values for all vertices
    for (int v = 0; v < num_vertices; ++v) {
        parent[v] = -1; // No parent an infinite length intailly
        key[v] = INF;    
        minHeap->array[v] = newMinHeapNode(v, key[v]);
        minHeap->pos[v] = v; // Position of each vertex in the heap
    }

    // Start with the first vertex (index 0) and set its key value to 0
    key[0] = 0;
    decreaseKey(minHeap, 0, key[0]);

    minHeap->size = num_vertices; // All vertices are in the heap initially

    // Iterate while the heap is not empty
    while (!isEmpty(minHeap)) {
        // Extract the vertex with the minimum key value
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // The vertex we just added to the MST

        // Update key values for all adjacent vertices
        for (int i = 0; i < num_edges[u]; i++) {
            int v = graph[u][i].to;
            int weight = graph[u][i].length;

            // If `v` is in the heap and the edge `u-v` is the smallest we've seen so far
            if (isInMinHeap(minHeap, v) && weight < key[v]) {
                key[v] = weight;  // Update the key value
                parent[v] = u;     // `u` becomes the parent of `v`
                decreaseKey(minHeap, v, key[v]); // Update the heap with the new key value
            }
        }
    }

    // Calculate and print the total weight of the MST
    int total_weight = 0;
    for (int i = 1; i < num_vertices; ++i) {
        total_weight += key[i];
    }

    // Free Memory
    free(parent);
    free(key);
    free(minHeap->pos);
    free(minHeap->array);
    free(minHeap);

    return total_weight;

}

int main(int argc, char *argv[]) { 
    int num_vertices, num_edges;

    // Read number of vertices and edges from input redirection
    if (scanf("%d %d", &num_vertices, &num_edges) != 2) {
        fprintf(stderr, "Error reading number of vertices and edges.\n");
        return 1;
    }

    // Allocate memory for the graph of edges
    struct Edge** graph = (struct Edge**)malloc(num_vertices * sizeof(struct Edge*));
    int* num_edges_per_vertex = (int*)malloc(num_vertices * sizeof(int));
    for (int i = 0; i < num_vertices; i++) {
        graph[i] = (struct Edge*)malloc(num_edges * sizeof(struct Edge)); // Allocate space for edges
        num_edges_per_vertex[i] = 0; // Initialize count of edges per vertex
    }

    // Read edges from the file using input redirection
    for (int i = 0; i < num_edges; i++) {
        int u, v, length;
        if (scanf("%d %d %d", &u, &v, &length) != 3) {
            fprintf(stderr, "Error reading edge.\n");
            return 1;
        }
        // Insert edge into the graph
        graph[u][num_edges_per_vertex[u]].to = v;
        graph[u][num_edges_per_vertex[u]].length = length;
        num_edges_per_vertex[u]++;
        
        // For an undirected graph, insert the reverse edge as well
        graph[v][num_edges_per_vertex[v]].to = u;
        graph[v][num_edges_per_vertex[v]].length = length;
        num_edges_per_vertex[v]++;
    }

    // Run Prim's algorithm to find the minimum spanning tree
    int mst_weight = primMST(graph, num_edges_per_vertex, num_vertices);

    // Output the total weight of the MST
    printf("%d\n", mst_weight);

    // Free allocated memory
    for (int i = 0; i < num_vertices; i++) {
        free(graph[i]);
    }
    free(graph);
    free(num_edges_per_vertex);

    return 0;

}