#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>
#include <stdbool.h>

typedef struct {
    float** matrix;
    float** matrixCopy;  
    int N;
    int startIdx;
    int endIdx;
    bool isErosion;
} ThreadData;


typedef struct {
    pthread_t* threads;
    ThreadData* threadData;
    int numThreads;
} ThreadPool;

float** createMatrix(int N , bool useFixedValues);
void printMatrix(float** matrix, int rows, int cols);
float** createMatrixCopy(float** matrix, int N);
void freeMatrix(float** matrix, int rows);
void applyFilterToSegment(float** matrix, float** matrixCopy, int N, int x, int y, bool isErosion);

ThreadPool* createThreadPool(int numThreads);
void destroyThreadPool(ThreadPool* pool);
void applyFilters(ThreadPool* pool, float** matrix, int N, int K, bool isErosion);

#endif // UTILS_H
