#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// Define a hash map entry for memoization
typedef struct DPEntry {
    char *assignment_str;  // Store the entire assignment as a string
    bool result;
    struct DPEntry *next;  // Handle collisions via chaining
} DPEntry;

#define HASH_MAP_SIZE 100003  // A large prime number for hash map size

DPEntry *hash_map[HASH_MAP_SIZE];  // Global hash map

// Hash function
unsigned long long hash(const char *assignment_str) {
    unsigned long long hash_value = 0;
    while (*assignment_str) {
        hash_value = (hash_value * 31) + *assignment_str++;
    }
    return hash_value % HASH_MAP_SIZE;
}

// Insert into hash map
void insert(const char *assignment_str, bool result) {
    unsigned long long h = hash(assignment_str);
    DPEntry *new_entry = (DPEntry *)malloc(sizeof(DPEntry));
    new_entry->assignment_str = strdup(assignment_str);  // Store a copy of the assignment string
    new_entry->result = result;
    new_entry->next = hash_map[h];
    hash_map[h] = new_entry;
}

// Search in hash map
bool search(const char *assignment_str, bool *result) {
    unsigned long long h = hash(assignment_str);
    DPEntry *current = hash_map[h];
    while (current) {
        if (strcmp(current->assignment_str, assignment_str) == 0) {
            *result = current->result;
            return true;
        }
        current = current->next;
    }
    return false;
}

// Create a string representation of the current assignment
char *get_assignment_str(int *assignments, int num_vars) {
    char *assignment_str = (char *)malloc(num_vars + 1);  // Extra byte for null terminator
    for (int i = 0; i < num_vars; i++) {
        assignment_str[i] = (assignments[i] == 1) ? '1' : (assignments[i] == 0 ? '0' : 'X');  // 'X' for unassigned
    }
    assignment_str[num_vars] = '\0';
    return assignment_str;
}

// Clause evaluation functions (unchanged)
bool evaluate_clause(int clause[3], int *assignments) {
    for (int i = 0; i < 3; i++) {
        int var = abs(clause[i]);
        int value = assignments[var - 1];
        if ((clause[i] > 0 && value == 1) || (clause[i] < 0 && value == 0)) {
            return true;  // Clause satisfied
        }
    }
    return false;  // Clause not satisfied
}

bool all_clauses_satisfied(int **clauses, int num_clauses, int *assignments) {
    for (int i = 0; i < num_clauses; i++) {
        if (!evaluate_clause(clauses[i], assignments)) {
            return false;
        }
    }
    return true;
}

bool any_clause_unsatisfied(int **clauses, int num_clauses, int *assignments) {
    for (int i = 0; i < num_clauses; i++) {
        bool unsat = true;
        for (int j = 0; j < 3; j++) {
            int var = abs(clauses[i][j]);
            int value = assignments[var - 1];
            if ((clauses[i][j] > 0 && value == 1) || (clauses[i][j] < 0 && value == 0)) {
                unsat = false;
                break;
            }
            if (value == -1) {
                unsat = false;
                break;
            }
        }
        if (unsat) return true;
    }
    return false;
}

// DPLL algorithm (modified for full assignment state tracking)
bool dpll(int **clauses, int num_clauses, int *assignments, int num_vars) {
    // Create a string representation of the current assignment
    char *assignment_str = get_assignment_str(assignments, num_vars);
    printf("now checking assignment str %s\n",assignment_str);
    // Check memoized result
    bool result;
    if (search(assignment_str, &result)) {
        free(assignment_str);  // Don't forget to free memory
        return result;
    }

    // Base cases
    if (all_clauses_satisfied(clauses, num_clauses, assignments)) {
        insert(assignment_str, true);
        free(assignment_str);
        return true;
    }
    if (any_clause_unsatisfied(clauses, num_clauses, assignments)) {
        // insert(assignment_str, false);
        // free(assignment_str);
        return false;
    }

    // Find the next unassigned variable
    int var = -1;
    for (int i = 0; i < num_vars; i++) {
        if (assignments[i] == -1) {
            var = i;
            break;
        }
    }
    if (var == -1) {
        //insert(assignment_str, false);
        //free(assignment_str);
        return false;  // No unassigned variables
    }

    // Try assigning true
    assignments[var] = 1;
    if (dpll(clauses, num_clauses, assignments, num_vars)) {
        insert(assignment_str, true);
        free(assignment_str);
        return true;
    }

    // Try assigning false
    assignments[var] = 0;
    if (dpll(clauses, num_clauses, assignments, num_vars)) {
        insert(assignment_str, true);
        free(assignment_str);
        return true;
    }

    // Backtrack
    assignments[var] = -1;
    //insert(assignment_str, false);
    //free(assignment_str);
    return false;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Open the input file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    int num_vars, num_clauses;
    fscanf(file, "%d %d", &num_vars, &num_clauses);

    // Allocate memory for clauses
    int **clauses = (int **)malloc(num_clauses * sizeof(int *));
    for (int i = 0; i < num_clauses; i++) {
        clauses[i] = (int *)malloc(3 * sizeof(int));
        fscanf(file, "%d %d %d", &clauses[i][0], &clauses[i][1], &clauses[i][2]);
    }
    fclose(file);

    // Initialize assignments (-1 means unassigned)
    int *assignments = (int *)malloc(num_vars * sizeof(int));
    for (int i = 0; i < num_vars; i++) {
        assignments[i] = -1;
    }

    // Start the timer
    clock_t start_time = clock();

    // Solve using DPLL
    bool is_satisfiable = dpll(clauses, num_clauses, assignments, num_vars);

    // Stop the timer
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    if (is_satisfiable) {
        printf("Satisfied\n");
        for (int i = 0; i < num_vars; i++) {
            printf("v%d = %s\n", i + 1, assignments[i] == 1 ? "true" : "false");
        }
    } else {
        printf("Unsatisfiable\n");
    }

    // Print the elapsed time
    printf("Time taken: %.6f seconds\n", elapsed_time);

    // Free memory
    for (int i = 0; i < num_clauses; i++) {
        free(clauses[i]);
    }
    free(clauses);
    free(assignments);

    return 0;
}