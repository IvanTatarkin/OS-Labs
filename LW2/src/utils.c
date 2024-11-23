#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

float** createMatrixCopy(float** matrix, int N) {
    float** copy = (float**)malloc(N * sizeof(float*));
    if (!copy) {
        fprintf(stderr, "Error allocating memory for matrix copy.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < N; i++) {
        copy[i] = (float*)malloc(N * sizeof(float));
        if (!copy[i]) {
            fprintf(stderr, "Error allocating memory for matrix row copy.\n");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < N; j++) {
            copy[i][j] = matrix[i][j];
        }
    }
    return copy;
}

float** createMatrix(int N, bool useFixedValues) {
    float** matrix = (float**)malloc(N * sizeof(float*));
    if (!matrix) {
        fprintf(stderr, "Error allocating memory for matrix.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < N; i++) {
        matrix[i] = (float*)malloc(N * sizeof(float));
        if (!matrix[i]) {
            fprintf(stderr, "Error allocating memory for matrix row.\n");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < N; j++) {
            if (useFixedValues) {
                matrix[i][j] = 1.0f + (i + j) % 2;
            } else {
                printf("Enter value for matrix[%d][%d]: ", i, j);
                scanf("%f", &matrix[i][j]);
            }
        }
    }
    return matrix;
}

void printMatrix(float** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%5.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void freeMatrix(float** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void applyFilterToSegment(float** matrix, float** matrixCopy, int N, int x, int y, bool isErosion) {
    float filterValue = matrix[x][y];
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < N && ny >= 0 && ny < N) {
                if (isErosion) {
                    filterValue = fminf(filterValue, matrix[nx][ny]);
                } else {
                    filterValue = fmaxf(filterValue, matrix[nx][ny]);
                }
            }
        }
    }
    matrixCopy[x][y] = filterValue;
}

void* processSegments(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    float** matrix = data->matrix;
    float** matrixCopy = data->matrixCopy;
    int N = data->N;
    int startIdx = data->startIdx;
    int endIdx = data->endIdx;
    bool isErosion = data->isErosion;

    for (int idx = startIdx; idx < endIdx; idx++) {
        int x = idx / N;
        int y = idx % N;
        applyFilterToSegment(matrix, matrixCopy, N, x, y, isErosion);
    }

    return NULL;
}

ThreadPool* createThreadPool(int numThreads) {
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    pool->threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    pool->threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    pool->numThreads = numThreads;
    return pool;
}

void destroyThreadPool(ThreadPool* pool) {
    free(pool->threads);
    free(pool->threadData);
    free(pool);
}

void applyFilters(ThreadPool* pool, float** matrix, int N, int K, bool isErosion) {
    for (int filter = 0; filter < K; filter++) {
        float** matrixCopy = createMatrixCopy(matrix, N);

        int numCells = N * N;
        int cellsPerThread = numCells / pool->numThreads;
        int extraCells = numCells % pool->numThreads;

        int startIdx = 0;
        for (int i = 0; i < pool->numThreads; i++) {
            int endIdx = startIdx + cellsPerThread + (i == pool->numThreads - 1 ? extraCells : 0);
            pool->threadData[i].matrix = matrix;
            pool->threadData[i].matrixCopy = matrixCopy;
            pool->threadData[i].N = N;
            pool->threadData[i].startIdx = startIdx;
            pool->threadData[i].endIdx = endIdx;
            pool->threadData[i].isErosion = isErosion;

            pthread_create(&pool->threads[i], NULL, processSegments, (void*)&pool->threadData[i]);
            startIdx = endIdx;
        }

        for (int i = 0; i < pool->numThreads; i++) {
            pthread_join(pool->threads[i], NULL);
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = matrixCopy[i][j];
            }
        }

        freeMatrix(matrixCopy, N);
    }
}
