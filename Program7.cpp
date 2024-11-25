//Joshua Klotzkin
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

typedef struct DPEntry {
    unsigned long long hash_value;
    bool result;
    struct DPEntry *next;
} DPEntry;

#define HASH_MAP_SIZE 100003  // Large prime for hash map size

DPEntry *hash_map[HASH_MAP_SIZE];  // Global hash map

// Hash function
unsigned long long hash(int *assignments, int num_vars) {
    unsigned long long hash_value = 0;
    for (int i = 0; i < num_vars; i++) {
        hash_value = (hash_value * 31) + (assignments[i] + 1);  // Encode -1, 0, 1 as 0, 1, 2
    }
    return hash_value % HASH_MAP_SIZE;
}

// Insert into hash map
void insert(int *assignments, int num_vars, bool result) {
    unsigned long long h = hash(assignments, num_vars);
    DPEntry *new_entry = (DPEntry *)malloc(sizeof(DPEntry));
    new_entry->hash_value = h;
    new_entry->result = result;
    new_entry->next = hash_map[h];
    hash_map[h] = new_entry;
}

// Search in hash map
bool search(int *assignments, int num_vars, bool *result) {
    unsigned long long h = hash(assignments, num_vars);
    DPEntry *current = hash_map[h];
    while (current) {
        if (current->hash_value == h) {
            *result = current->result;
            return true;
        }
        current = current->next;
    }
    return false;
}

// Evaluate a single clause
bool evaluate_clause(int clause[3], int *assignments) {
    for (int i = 0; i < 3; i++) {
        int var = abs(clause[i]);
        int value = assignments[var - 1];
        if ((clause[i] > 0 && value == 1) || (clause[i] < 0 && value == 0)) {
            return true;
        }
    }
    return false;
}

// Check if all clauses are satisfied
bool all_clauses_satisfied(int **clauses, int num_clauses, int *assignments) {
    for (int i = 0; i < num_clauses; i++) {
        if (!evaluate_clause(clauses[i], assignments)) {
            return false;
        }
    }
    return true;
}

// Check for unsatisfied clauses
bool any_clause_unsatisfied(int **clauses, int num_clauses, int *assignments) {
    for (int i = 0; i < num_clauses; i++) {
        if (!evaluate_clause(clauses[i], assignments)) {
            bool partially_unsat = true;
            for (int j = 0; j < 3; j++) {
                int var = abs(clauses[i][j]);
                //printf("Evaluating var %d of clause #%d currently %d\n",clauses[i][j],i+1,assignments[var - 1]);
                if (assignments[var - 1] == -1) {
                    partially_unsat = false;
                    break;
                }
            }
            if (partially_unsat) return true;
        }
    }
    return false;
}

// Most Constrained Variable heuristic
int select_most_constrained_variable(int **clauses, int num_clauses, int *assignments, int num_vars) {
    int *freq = (int *)calloc(num_vars, sizeof(int));

    // Count frequency of unassigned variables in unresolved clauses
    for (int i = 0; i < num_clauses; i++) {
        for (int j = 0; j < 3; j++) {
            int var = abs(clauses[i][j]) - 1;
            if (assignments[var] == -1) {
                freq[var]++;
            }
        }
    }

    // Find the variable with the highest frequency
    int max_freq = -1;
    int chosen_var = -1;
    for (int i = 0; i < num_vars; i++) {
        if (assignments[i] == -1 && freq[i] >= max_freq) {
            max_freq = freq[i];
            chosen_var = i;
        }
    }

    free(freq);
    printf("%d is currently the most problemmatic\n",chosen_var+1);
    return chosen_var;
}

// DPLL algorithm with memoization and heuristic
bool dpll(int **clauses, int num_clauses, int *assignments, int num_vars) {
    // Check memoized result
    bool result;
    if (search(assignments, num_vars, &result)) {
        printf("Memoized result found for current assignment. Returning: %s\n", result ? "true" : "false");
        return result;
    }

    // Base cases
    if (all_clauses_satisfied(clauses, num_clauses, assignments)) {
        printf("All clauses satisfied with current assignment. Returning: true\n");
        insert(assignments, num_vars, true);
        return true;
    }
    if (any_clause_unsatisfied(clauses, num_clauses, assignments)) {
        printf("At least one clause unsatisfied with current assignment. Returning: false\n");
        //insert(assignments, num_vars, false);
        return false;
    }

    // Select the next variable using heuristic
    int var = select_most_constrained_variable(clauses, num_clauses, assignments, num_vars);

    if (var == -1) {
        printf("No variable selected. Likely no unassigned variables remain. Returning: false\n");
        return false;
    }

    // Try assigning true
    printf("Setting variable %d to true and attempting recursion.\n", var);
    assignments[var] = 1;
    if (dpll(clauses, num_clauses, assignments, num_vars)) {
        printf("Recursion successful with variable %d set to true. Returning: true\n", var);
        insert(assignments, num_vars, true);
        return true;
    }

    // Try assigning false
    printf("Setting variable %d to false and attempting recursion.\n", var);
    assignments[var] = 0;
    if (dpll(clauses, num_clauses, assignments, num_vars)) {
        printf("Recursion successful with variable %d set to false. Returning: true\n", var);
        insert(assignments, num_vars, true);
        return true;
    }

    // Backtrack
    printf("Backtracking on variable %d. Setting to unassigned (-1).\n", var);
    assignments[var] = -1;
    if (dpll(clauses, num_clauses, assignments, num_vars)) {
        printf("Backtracking successful with variable %d set to -1. Returning: true\n", var);
        insert(assignments, num_vars, true);
        return true;
    }
    insert(assignments, num_vars, false);
    printf("Backtracked completely on variable %d. Returning: false\n", var);
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
    memset(assignments, -1, num_vars * sizeof(int));

    clock_t start_time = clock();

    // Solve using DPLL
    bool is_satisfiable = dpll(clauses, num_clauses, assignments, num_vars);

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

    printf("Time taken: %.6f seconds\n", elapsed_time);

    // Free memory
    for (int i = 0; i < num_clauses; i++) {
        free(clauses[i]);
    }
    free(clauses);
    free(assignments);

    return 0;
}
