// Joshua Klotzkin
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

// Define structures as before
typedef struct {
    int x, y, length;
} Block;

typedef struct {
    int x, y;
} Row;

typedef struct {
    int min_cost;
    int *best_row1_blocks;
    int *best_row2_blocks;
    int best_row1_count;
    int best_row2_count;
} Result;

// Recursive function with memoization
int find_min_cost(Block *blocks, int num_blocks, int index, Row row1, Row row2, 
                  int row1_length, int row2_length, int target_length, int current_cost, 
                  int *row1_blocks, int row1_count, int *row2_blocks, int row2_count, 
                  Result *result, int ***dp) {

    // Check if we have reached the last block
    if (index == num_blocks) {
        if (current_cost < result->min_cost) {
            result->min_cost = current_cost;
            result->best_row1_count = row1_count;
            result->best_row2_count = row2_count;
            for (int i = 0; i < row1_count; i++) result->best_row1_blocks[i] = row1_blocks[i];
            for (int i = 0; i < row2_count; i++) result->best_row2_blocks[i] = row2_blocks[i];
        }
        return current_cost;
    }

    // Memoization check: return if this state is already computed
    if (dp[index][row1_length][row2_length] <= current_cost) return dp[index][row1_length][row2_length];

    Block block = blocks[index];
    int cost_with_row1 = INT_MAX, cost_with_row2 = INT_MAX;

    // Try placing block in row 1 if it fits
    if (row1_length + block.length <= target_length) {
        int cost_row1 = pow(block.x - row1.x, 2) + pow(block.y - row1.y, 2);
        row1_blocks[row1_count] = index;
        Row new_row1 = {row1.x + block.length, row1.y};
        //printf("Putting Block %d in row 0 for an added cost of %d to the current cost of %d total cost of %d\n",index,cost_row1,current_cost,current_cost+cost_row1);
        if(index==num_blocks-1)
        {
            //printf("This is the last block placed on this side of the tree making the final cost %d\n",current_cost+cost_row1);
        }
        cost_with_row1 = find_min_cost(blocks, num_blocks, index + 1, new_row1, row2,
                                       row1_length + block.length, row2_length, target_length,
                                       current_cost + cost_row1, row1_blocks, row1_count + 1,
                                       row2_blocks, row2_count, result, dp);
    }

    // Try placing block in row 2 if it fits
    if (row2_length + block.length <= target_length) {
        int cost_row2 = pow(block.x - row2.x, 2) + pow(block.y - row2.y, 2);
        row2_blocks[row2_count] = index;
        Row new_row2 = {row2.x + block.length, row2.y};
        //printf("Putting Block %d in row 0 for an added cost of %d to the current cost of %d total cost of %d\n",index,cost_row2,current_cost,current_cost+cost_row2);
        if(index==num_blocks-1)
        {
            //printf("This is the last block placed on this side of the tree making the final cost %d\n",current_cost+cost_row2);
        }
        cost_with_row2 = find_min_cost(blocks, num_blocks, index + 1, row1, new_row2,
                                       row1_length, row2_length + block.length, target_length,
                                       current_cost + cost_row2, row1_blocks, row1_count,
                                       row2_blocks, row2_count + 1, result, dp);
    }

    // Memoize the minimum cost of placing the block in either row
    dp[index][row1_length][row2_length] = fmin(cost_with_row1, cost_with_row2);
    return dp[index][row1_length][row2_length];
}

void print_table(Result *result, Block *blocks, int num_blocks) {

    // Display all blocks and where they were moved
    for (int i = 0; i < result->best_row1_count; i++) {
        int block_idx = result->best_row1_blocks[i];
        Block block = blocks[block_idx];
        printf("Block %d (length %d) on row 1\n", block_idx + 1, block.length);
    }

    for (int i = 0; i < result->best_row2_count; i++) {
        int block_idx = result->best_row2_blocks[i];
        Block block = blocks[block_idx];
        printf("Block %d (length %d) on row 2\n", block_idx + 1, block.length);
    }

    printf("\n");

    // Now, print the details for each row
    printf("Row 1 placements:\n");
    int row1_length = 0;
    for (int i = 0; i < result->best_row1_count; i++) {
        int block_idx = result->best_row1_blocks[i];
        Block block = blocks[block_idx];
        
        int cost = pow(block.x - row1.x, 2) + pow(block.y - row1.y, 2);
        printf("Block %d ( %3d, %3d) -> ( %3d, %3d) cost %d\n", block_idx + 1, row1.x, row1.y, row1.x + block.length, row1.y);
        
        // Update row position after placing the block
        row1.x += block.length;
        row1_length += block.length;
    }

    printf("Row 1 total length: %d\n", row1_length);

    printf("\nRow 2 placements:\n");
    int row2_length = 0;
    for (int i = 0; i < result->best_row2_count; i++) {
        int block_idx = result->best_row2_blocks[i];
        Block block = blocks[block_idx];

        int cost = pow(block.x - row2.x, 2) + pow(block.y - row2.y, 2);
        printf("Block %d ( %3d, %3d) -> ( %3d, %3d) cost %d\n", block_idx + 1, row2.x, row2.y, row2.x + block.length, row2.y);
        
        // Update row position after placing the block
        row2.x += block.length;
        row2_length += block.length;
    }

    printf("Row 2 total length: %d\n", row2_length);
    printf("Total cost: %d\n", result->min_cost);
}



int main() {
    Row row1, row2;
    int num_blocks;

    if (scanf("%d %d", &row1.x, &row1.y) != 2 || scanf("%d %d", &row2.x, &row2.y) != 2) {
        fprintf(stderr, "Error reading row coordinates.\n");
        return 1;
    }

    if (scanf("%d", &num_blocks) != 1) {
        fprintf(stderr, "Error reading number of blocks.\n");
        return 1;
    }

    Block *blocks = malloc(num_blocks * sizeof(Block));
    if (blocks == NULL) {
        perror("Failed to allocate memory for blocks");
        return 1;
    }

    int total_length = 0;
    for (int i = 0; i < num_blocks; i++) {
        if (scanf("%d %d %d", &blocks[i].x, &blocks[i].y, &blocks[i].length) != 3) {
            fprintf(stderr, "Error reading block data.\n");
            free(blocks);
            return 1;
        }
        total_length += blocks[i].length;
    }

    int target_length = total_length / 2;
    printf("Target Length: %d\n", target_length);
    Result result = { .min_cost = INT_MAX };
    result.best_row1_blocks = malloc(num_blocks * sizeof(int));
    result.best_row2_blocks = malloc(num_blocks * sizeof(int));
    int *row1_blocks = malloc(num_blocks * sizeof(int));
    int *row2_blocks = malloc(num_blocks * sizeof(int));

    // Initialize DP table
    int ***dp = malloc((num_blocks + 1) * sizeof(int**));
    for (int i = 0; i <= num_blocks; i++) {
        dp[i] = malloc((target_length + 1) * sizeof(int*));
        for (int j = 0; j <= target_length; j++) {
            dp[i][j] = malloc((target_length + 1) * sizeof(int));
            for (int k = 0; k <= target_length; k++) dp[i][j][k] = INT_MAX;
        }
    }

    find_min_cost(blocks, num_blocks, 0, row1, row2, 0, 0, target_length, 0, row1_blocks, 0, row2_blocks, 0, &result, dp);

    printf("Minimum Cost: %d\n", result.min_cost);
    print_table(&result, blocks, num_blocks);

    for (int i = 0; i <= num_blocks; i++) {
        for (int j = 0; j <= target_length; j++) free(dp[i][j]);
        free(dp[i]);
    }
    free(dp);
    free(blocks);
    free(row1_blocks);
    free(row2_blocks);
    free(result.best_row1_blocks);
    free(result.best_row2_blocks);

    return 0;
}
