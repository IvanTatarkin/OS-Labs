#include <gtest/gtest.h>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <utility>
#include <fstream>

extern "C" {
    #include "utils.h" 
}

int executeProgram(const char* command) {
    return system(command);
}

TEST(ConvolutionTest, ParameterCheck) {

    int result = executeProgram("../lab2/lab2_exe 5 0 1 2");
    ASSERT_NE(result, 0);

    result = executeProgram("../lab2/lab2_exe 0 3 1 2");
    ASSERT_NE(result, 0);

    result = executeProgram("../lab2/lab2_exe 5 6 1 2");
    ASSERT_NE(result, 0);

    result = executeProgram("../lab2/lab2_exe 5 5 2 1");
    ASSERT_NE(result, 0);

    result = executeProgram("../lab2/lab2_exe 5 3 0 2");
    ASSERT_NE(result, 0);

    result = executeProgram("../lab2/lab2_exe 5 3 1 0");
    ASSERT_NE(result, 0);
}


TEST(ConvolutionTest, Correctness) {
    int N = 5;
    int K = 3;
    int maxThreads = 2;
    
    bool isErosion = true;  
    float** matrix = createMatrix(N, 1);
    printMatrix(matrix, N, N);

    ThreadPool* pool = createThreadPool(maxThreads);

    applyFilters(pool, matrix, N, K, isErosion);

    float expectedErosion[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };

    bool isCorrect = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(matrix[i][j] - expectedErosion[i][j]) > 1e-6) {
                isCorrect = false;
                printf("Mismatch at (%d, %d): Expected %.2f, got %.2f\n", i, j, expectedErosion[i][j], matrix[i][j]);
            }
        }
    }

    ASSERT_TRUE(isCorrect);

    freeMatrix(matrix, N);


    matrix = createMatrix(N, 1);

    isErosion = false;

    applyFilters(pool, matrix, N, K, isErosion);

    float expectedDilation[5][5] = {
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2}
    };

    isCorrect = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(matrix[i][j] - expectedDilation[i][j]) > 1e-6) {
                isCorrect = false;
                printf("Mismatch at (%d, %d): Expected %.2f, got %.2f\n", i, j, expectedDilation[i][j], matrix[i][j]);
            }
        }
    }

    ASSERT_TRUE(isCorrect);

    freeMatrix(matrix, N);
    destroyThreadPool(pool);
}

TEST(ConvolutionTest, Correctness2) {
    int N = 5;
    int K = 1;
    int maxThreads = 2;
    
    bool isErosion = true;  
    float** matrix = createMatrix(N, 1);
    printMatrix(matrix, N, N);

    ThreadPool* pool = createThreadPool(maxThreads);

    applyFilters(pool, matrix, N, K, isErosion);

    float expectedErosion[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };

    bool isCorrect = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(matrix[i][j] - expectedErosion[i][j]) > 1e-6) {
                isCorrect = false;
                printf("Mismatch at (%d, %d): Expected %.2f, got %.2f\n", i, j, expectedErosion[i][j], matrix[i][j]);
            }
        }
    }

    ASSERT_TRUE(isCorrect);

    freeMatrix(matrix, N);


    matrix = createMatrix(N, 1);

    isErosion = false;

    applyFilters(pool, matrix, N, K, isErosion);

    float expectedDilation[5][5] = {
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2}
    };

    isCorrect = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(matrix[i][j] - expectedDilation[i][j]) > 1e-6) {
                isCorrect = false;
                printf("Mismatch at (%d, %d): Expected %.2f, got %.2f\n", i, j, expectedDilation[i][j], matrix[i][j]);
            }
        }
    }

    ASSERT_TRUE(isCorrect);

    freeMatrix(matrix, N);
    destroyThreadPool(pool);
}
