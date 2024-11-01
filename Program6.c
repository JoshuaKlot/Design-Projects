#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
    int x, y, length;
} Element;

int calculate_cost(int x1, int y1, int x2, int y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void dual_row_placement() {
    int row1_x, row1_y, row2_x, row2_y, n;
    scanf("%d %d", &row1_x, &row1_y);
    scanf("%d %d", &row2_x, &row2_y);
    scanf("%d", &n);

    Element elements[n];
    int total_length = 0;
    
    for (int i = 0; i < n; i++) {
        scanf("%d %d %d", &elements[i].x, &elements[i].y, &elements[i].length);
        total_length += elements[i].length;
    }

    int target_length = total_length / 2;
    
    // Initialize DP table with high values
    int dp[n+1][target_length+1];
    int path[n+1][target_length+1];
    
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= target_length; j++) {
            dp[i][j] = INT_MAX;
            path[i][j] = -1;
        }
    }
    
    dp[0][0] = 0;
    
    for (int i = 0; i < n; i++) {
        int cost_row1 = calculate_cost(elements[i].x, elements[i].y, row1_x, row1_y);
        int cost_row2 = calculate_cost(elements[i].x, elements[i].y, row2_x, row2_y);
        
        for (int j = 0; j <= target_length; j++) {
            if (dp[i][j] == INT_MAX) continue;

            // Option 1: Place element in the first row (if it fits)
            if (j + elements[i].length <= target_length) {
                int new_cost = dp[i][j] + cost_row1;
                if (new_cost < dp[i+1][j + elements[i].length]) {
                    dp[i+1][j + elements[i].length] = new_cost;
                    path[i+1][j + elements[i].length] = 0; // 0 means row 1
                }
            }

            // Option 2: Place element in the second row
            int new_cost = dp[i][j] + cost_row2;
            if (new_cost < dp[i+1][j]) {
                dp[i+1][j] = new_cost;
                path[i+1][j] = 1; // 1 means row 2
            }
        }
    }

    int min_cost = dp[n][target_length];
    printf("%d\n", min_cost);

    // Backtrack to determine which row each element was placed in
    int row1[n], row2[n];
    int row1_count = 0, row2_count = 0;
    int j = target_length;

    for (int i = n; i > 0; i--) {
        if (path[i][j] == 0) {
            row1[row1_count++] = i - 1;
            j -= elements[i - 1].length;
        } else {
            row2[row2_count++] = i - 1;
        }
    }

    // Print row 1
    for (int i = row1_count - 1; i >= 0; i--) {
        printf("%d ", row1[i]);
    }
    printf("\n");

    // Print row 2
    for (int i = row2_count - 1; i >= 0; i--) {
        printf("%d ", row2[i]);
    }
    printf("\n");
}

int main() {
    dual_row_placement();
    return 0;
}
