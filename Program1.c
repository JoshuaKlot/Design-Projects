//Joshua Klotzkin
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to swap two elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Hoare partition scheme
int partition(int arr[], int low, int high) {
    //Set up a random pivot
    int pivotIndex = low + rand() % (high - low + 1);
    int pivot = arr[pivotIndex];

    //Set the pivot as the first element
    swap(&arr[pivotIndex], &arr[low]);

    //iniatilize the indexes
    int i = low - 1;
    int j = high + 1;

    while (1) {
        // Finding leftmost element greater than or equal to pivot
        do {
            i++;
        } while (arr[i] < pivot);

        // Finding rightmost element smaller than or equal to pivot
        do {
            j--;
        } while (arr[j] > pivot);

        //When the two pointers meet ....
        if (i >= j)
            return j;
        //Swap them
        swap(&arr[i], &arr[j]);
    }
}

// QuickSort function
void quicksort(int arr[], int low, int high) {
    if (low < high) {
        //Finding the partion
        int p = partition(arr, low, high);

        //sort the elements before and after the partition, recursively
        quicksort(arr, low, p);
        quicksort(arr, p + 1, high);
    }
}

// Main function
int main(int argc, char *argv[]) {

    //Gives an error if the amount of the arguements arent 1 or 3
    if (argc != 1 && argc != 3) {
        printf("Usage: %s <rangeStart> <rangeEnd> < inputfile\n", argv[0]);
        return 1;
    }

    int rangeStart = 0;
    int rangeEnd = 0;
    int useRange = 0;
    //Set up the range if there are 3 arguements to set it up
    if (argc == 3) {
        rangeStart = atoi(argv[1]);
        rangeEnd = atoi(argv[2]);
        useRange = 1;
    }

    //Scan the file and allocate memory for it.
    int n;
    scanf("%d", &n); 

    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        perror("Error allocating memory");
        return 1;
    }
    
    //Start the quick sort
    quicksort(arr, 0, n - 1);
    // If use range is 1 print out the numbers in range rangestart to rangeend
    if (useRange) {
        printf("Total numbers in range [%d, %d]: %d\n", rangeStart, rangeEnd, rangeEnd - rangeStart);
        for (int i = rangeStart; i < rangeEnd && i < n; i++) {
            printf("%d ", arr[i]);
            printf("\n");
        }
    } else {
        //Otherwise print out the whole thing
        printf("Sorted array:\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", arr[i]);
            printf("\n");
        }
    }
    printf("\n");

    //Free the memory previously allocated to the array
    free(arr);
    return 0;
}
