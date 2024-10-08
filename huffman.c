#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of letters allowed
#define MAX_LETTERS 100

// Node structure for Huffman Tree
typedef struct Node {
    int frequency;      // Frequency of the node
    int id;             // Identifier (0 to 99)
    struct Node* left;  // Left child
    struct Node* right; // Right child
} Node;

// Min-Heap structure
typedef struct MinHeap {
    int size;       // Current number of elements in heap
    int capacity;   // Maximum capacity of heap
    Node** array;   // Array of node pointers
} MinHeap;

// Function prototypes
Node* createNode(int id, int frequency);
MinHeap* createMinHeap(int capacity);
void swapNodes(Node** a, Node** b);
void minHeapify(MinHeap* heap, int idx);
Node* extractMin(MinHeap* heap);
void insertMinHeap(MinHeap* heap, Node* node);
MinHeap* buildMinHeap(int* frequencies, int n);
Node* buildHuffmanTree(int* frequencies, int n);
void calculateBitLengths(Node* root, int currentLength, int* bitLengths);
void freeTree(Node* root);

// Helper function to create a new node
Node* createNode(int id, int frequency) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed for new node.\n");
        exit(1);
    }
    newNode->id = id;
    newNode->frequency = frequency;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Helper function to create a min-heap of given capacity
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!minHeap) {
        fprintf(stderr, "Memory allocation failed for min heap.\n");
        exit(1);
    }
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (Node**)malloc(minHeap->capacity * sizeof(Node*));
    if (!minHeap->array) {
        fprintf(stderr, "Memory allocation failed for heap array.\n");
        exit(1);
    }
    return minHeap;
}

// Helper function to swap two nodes in the heap
void swapNodes(Node** a, Node** b) {
    Node* temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify the node at index 'idx'
void minHeapify(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->array[left]->frequency < heap->array[smallest]->frequency)
        smallest = left;

    if (right < heap->size && heap->array[right]->frequency < heap->array[smallest]->frequency)
        smallest = right;

    if (smallest != idx) {
        swapNodes(&heap->array[smallest], &heap->array[idx]);
        minHeapify(heap, smallest);
    }
}

// Extract the node with the minimum frequency
Node* extractMin(MinHeap* heap) {
    if (heap->size == 0)
        return NULL;

    Node* temp = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    minHeapify(heap, 0);
    return temp;
}

// Insert a new node into the heap
void insertMinHeap(MinHeap* heap, Node* node) {
    if (heap->size == heap->capacity) {
        fprintf(stderr, "Heap overflow. Cannot insert more nodes.\n");
        exit(1);
    }

    // Insert the new node at the end
    int i = heap->size;
    heap->array[i] = node;
    heap->size++;

    // Fix the min heap property if it's violated
    while (i != 0 && heap->array[i]->frequency < heap->array[(i - 1) / 2]->frequency) {
        swapNodes(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Build a min-heap from an array of frequencies
MinHeap* buildMinHeap(int* frequencies, int n) {
    MinHeap* heap = createMinHeap(n);
    for (int i = 0; i < n; ++i) {
        heap->array[i] = createNode(i, frequencies[i]);
    }
    heap->size = n;

    // Build the heap (min-heapify from the last non-leaf node)
    for (int i = (heap->size - 1) / 2; i >= 0; --i) {
        minHeapify(heap, i);
    }

    return heap;
}

// Build the Huffman Tree from the given frequencies
Node* buildHuffmanTree(int* frequencies, int n) {
    if (n == 0)
        return NULL;

    Node *left, *right, *top;

    // Create a min heap with all the letters
    MinHeap* heap = buildMinHeap(frequencies, n);

    // Iterate until the heap contains only one node
    while (heap->size > 1) {
        // Extract the two nodes with the smallest frequencies
        left = extractMin(heap);
        right = extractMin(heap);

        // Create a new internal node with frequency equal to the sum of the two nodes
        top = createNode(-1, left->frequency + right->frequency);
        top->left = left;
        top->right = right;

        // Insert the new node back into the min heap
        insertMinHeap(heap, top);
    }

    // The remaining node is the root of the Huffman tree
    Node* root = extractMin(heap);
    free(heap->array);
    free(heap);
    return root;
}

// Traverse the Huffman tree and calculate bit lengths for each letter
void calculateBitLengths(Node* root, int currentLength, int* bitLengths) {
    if (root == NULL)
        return;

    // If it's a leaf node, assign the current bit length
    if (root->left == NULL && root->right == NULL) {
        // Handle the case where there's only one letter
        if (currentLength == 0)
            bitLengths[root->id] = 1;
        else
            bitLengths[root->id] = currentLength;
        return;
    }

    // Recur for left and right children
    calculateBitLengths(root->left, currentLength + 1, bitLengths);
    calculateBitLengths(root->right, currentLength + 1, bitLengths);
}

// Free the memory allocated for the Huffman tree
void freeTree(Node* root) {
    if (root == NULL)
        return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// Main function
int main() {
    int numLetters;

    // Read number of letters
    if (scanf("%d", &numLetters) != 1 || numLetters <= 0 || numLetters > MAX_LETTERS) {
        fprintf(stderr, "Invalid input: Number of letters must be between 1 and %d.\n", MAX_LETTERS);
        return 1;
    }

    // Read frequencies of the letters
    int frequencies[MAX_LETTERS] = {0};
    for (int i = 0; i < numLetters; ++i) {
        if (scanf("%d", &frequencies[i]) != 1 || frequencies[i] < 0) {
            fprintf(stderr, "Invalid input: Frequencies must be non-negative integers.\n");
            return 1;
        }
    }

    // Handle the special case where there is only one letter
    if (numLetters == 1) {
        printf("%d\n", frequencies[0]); // Each occurrence uses 1 bit
        return 0;
    }

    // Build the Huffman tree
    Node* root = buildHuffmanTree(frequencies, numLetters);

    // Array to store bit lengths for each letter
    int bitLengths[MAX_LETTERS] = {0};

    // Calculate bit lengths for each letter
    calculateBitLengths(root, 0, bitLengths);

    // Calculate total bits required
    long long totalBits = 0; // Using long long to prevent integer overflow for large inputs
    for (int i = 0; i < numLetters; ++i) {
        totalBits += (long long)frequencies[i] * bitLengths[i];
    }

    // Print the total bits required
    printf("%lld\n", totalBits);

    // Free the Huffman tree
    freeTree(root);

    return 0;
}
