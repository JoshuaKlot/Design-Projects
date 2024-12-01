// Joshua Klotzkin
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

// Structure to represent a stop
typedef struct {
    float x, y;
} Stop;

// Function to calculate the Euclidean distance between two stops
float distance(Stop a, Stop b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Find the nearest unvisited stop
int find_nearest(int current, Stop *stops, int *visited, int num_stops) {
    float min_distance = FLT_MAX;
    int nearest_stop = -1;

    for (int i = 0; i < num_stops; i++) {
        if (!visited[i]) { // Only consider unvisited stops
            float dist = distance(stops[current], stops[i]);
            if (dist < min_distance) {
                min_distance = dist;
                nearest_stop = i;
            }
        }
    }
    return nearest_stop;
}

// Nearest Neighbor Heuristic to solve TSP
void solve_tsp(Stop *stops, int num_stops, int *tour, float *total_distance) {
    int *visited = (int *)calloc(num_stops, sizeof(int));
    int current = 0; // Start at the first stop
    visited[current] = 1;
    tour[0] = current;

    *total_distance = 0;
    for (int i = 1; i < num_stops; i++) {
        int next_stop = find_nearest(current, stops, visited, num_stops);
        *total_distance += distance(stops[current], stops[next_stop]);
        visited[next_stop] = 1;
        tour[i] = next_stop;
        current = next_stop;
    }
    // Return to the start to complete the tour
    *total_distance += distance(stops[current], stops[tour[0]]);

    free(visited);
}

// Function to perform 2-Opt swap to improve the tour
void two_opt(Stop *stops, int num_stops, int *tour, float *total_distance) {
    int improved = 1;
    while (improved) {
        improved = 0;
        for (int i = 1; i < num_stops - 2; i++) {
            for (int j = i + 1; j < num_stops - 1; j++) {
                if (j - i == 1) continue; // Avoid adjacent points
                float old_distance = distance(stops[tour[i - 1]], stops[tour[i]]) + distance(stops[tour[j]], stops[tour[j + 1]]);
                float new_distance = distance(stops[tour[i - 1]], stops[tour[j]]) + distance(stops[tour[i]], stops[tour[j + 1]]);

                if (new_distance < old_distance) {
                    // Swap the two segments
                    int temp;
                    for (int k = 0; k <= (j - i) / 2; k++) {
                        temp = tour[i + k];
                        tour[i + k] = tour[j - k];
                        tour[j - k] = temp;
                    }
                    *total_distance += new_distance - old_distance;
                    improved = 1;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Open input file
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    // Read the number of stops
    int num_stops;
    fscanf(input, "%d", &num_stops);

    // Allocate memory for stops
    Stop *stops = (Stop *)malloc(num_stops * sizeof(Stop));
    for (int i = 0; i < num_stops; i++) {
        fscanf(input, "%f %f", &stops[i].x, &stops[i].y);
    }
    fclose(input);

    // Allocate memory for the tour and solve TSP using the Nearest Neighbor heuristic
    int *tour = (int *)malloc(num_stops * sizeof(int));
    float total_distance;
    solve_tsp(stops, num_stops, tour, &total_distance);

    // Improve the tour using 2-Opt algorithm
    two_opt(stops, num_stops, tour, &total_distance);

    // Write results to the output file "results.txt"
    FILE *output = fopen("output.txt", "w");
    if (!output) {
        perror("Error opening output file");
        free(stops);
        free(tour);
        return 1;
    }

    fprintf(output, "%d\n", num_stops);
    for (int i = 0; i < num_stops; i++) {
        fprintf(output, "%d ", tour[i]); // Use 1-based indexing for output
    }
    fprintf(output, "\nTotal Distance: %.2f\n", total_distance);
    fclose(output);

    // Free allocated memory
    free(stops);
    free(tour);

    return 0;
}
