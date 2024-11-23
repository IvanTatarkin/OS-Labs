#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <N> <K> <MAX_THREADS>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int K = atoi(argv[2]);
    int maxThreads = atoi(argv[3]);

    if (N <= 0 || K <= 0 || maxThreads <= 0) {
        fprintf(stderr, "Error: Matrix size, number of filters, and threads must be greater than 0.\n");
        return 1;
    }

    printf("Matrix size: %d\n", N);
    printf("Number of filters (K): %d\n", K);
    printf("Max threads: %d\n", maxThreads);

    clock_t start = clock();

    float** matrix = createMatrix(N, 0);
    if (N < 25) {
        printf("\nOriginal matrix:\n");
        printMatrix(matrix, N, N);
    }

    float** matrixDilation = createMatrixCopy(matrix, N);
    float** matrixErosion = createMatrixCopy(matrix, N);

    ThreadPool* pool = createThreadPool(maxThreads);

    printf("\nApplying Dilation filter:\n");
    applyFilters(pool, matrixDilation, N, K, false);

    if (N < 25) {
        printf("\nMatrix after Dilation (After %d passes):\n", K);
        printMatrix(matrixDilation, N, N);
    }

    printf("\nApplying Erosion filter:\n");
    applyFilters(pool, matrixErosion, N, K, true);

    if (N < 25) {
        printf("\nMatrix after Erosion (After %d passes):\n", K);
        printMatrix(matrixErosion, N, N);
    }

    destroyThreadPool(pool);

    freeMatrix(matrix, N);
    freeMatrix(matrixDilation, N);
    freeMatrix(matrixErosion, N);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\nTime spent: %.5f seconds\n", time_spent);

    return 0;
}
