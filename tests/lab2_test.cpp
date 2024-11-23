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
    int N = 5;  // Размер матрицы
    int K = 3;  // Количество фильтров
    int maxThreads = 2;  // Максимальное количество потоков (для многозадачности)
    
    // Проверка для эрозии
    bool isErosion = true;  
    float** matrix = createMatrix(N, 1);  // Создаем исходную матрицу
    printMatrix(matrix, N, N);

    ThreadPool* pool = createThreadPool(maxThreads);  // Создаем пул потоков

    // Применяем фильтры для эрозии
    applyFilters(pool, matrix, N, K, isErosion);

    // Ожидаемая матрица после эрозии (примерный результат)
    float expectedErosion[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };

    // Сравниваем полученную матрицу с ожидаемой
    bool isCorrect = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(matrix[i][j] - expectedErosion[i][j]) > 1e-6) {  // Проверка с точностью до 1e-6
                isCorrect = false;
                printf("Mismatch at (%d, %d): Expected %.2f, got %.2f\n", i, j, expectedErosion[i][j], matrix[i][j]);
            }
        }
    }

    ASSERT_TRUE(isCorrect);  // Проверяем, что все элементы совпадают с ожидаемыми для эрозии

    // Освобождение памяти
    freeMatrix(matrix, N);

    // Теперь проверим для наращивания

    // Создаем новую матрицу для наращивания
    matrix = createMatrix(N, 1);

    isErosion = false;  // Для наращивания

    // Применяем фильтры для наращивания
    applyFilters(pool, matrix, N, K, isErosion);

    // Ожидаемая матрица после наращивания
    float expectedDilation[5][5] = {
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2}
    };

    // Сравниваем полученную матрицу с ожидаемой для наращивания
    isCorrect = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(matrix[i][j] - expectedDilation[i][j]) > 1e-6) {
                isCorrect = false;
                printf("Mismatch at (%d, %d): Expected %.2f, got %.2f\n", i, j, expectedDilation[i][j], matrix[i][j]);
            }
        }
    }

    ASSERT_TRUE(isCorrect);  // Проверяем, что все элементы совпадают с ожидаемыми для наращивания

    // Освобождение памяти
    freeMatrix(matrix, N);
    destroyThreadPool(pool);  // Освобождаем ресурсы пула потоков
}

TEST(ConvolutionTest, Correctness2) {
    int N = 5;  // Размер матрицы
    int K = 1;  // Количество фильтров
    int maxThreads = 2;  // Максимальное количество потоков (для многозадачности)
    
    // Проверка для эрозии
    bool isErosion = true;  
    float** matrix = createMatrix(N, 1);  // Создаем исходную матрицу
    printMatrix(matrix, N, N);

    ThreadPool* pool = createThreadPool(maxThreads);  // Создаем пул потоков

    // Применяем фильтры для эрозии
    applyFilters(pool, matrix, N, K, isErosion);

    // Ожидаемая матрица после эрозии (примерный результат)
    float expectedErosion[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };

    // Сравниваем полученную матрицу с ожидаемой
    bool isCorrect = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(matrix[i][j] - expectedErosion[i][j]) > 1e-6) {  // Проверка с точностью до 1e-6
                isCorrect = false;
                printf("Mismatch at (%d, %d): Expected %.2f, got %.2f\n", i, j, expectedErosion[i][j], matrix[i][j]);
            }
        }
    }

    ASSERT_TRUE(isCorrect);  // Проверяем, что все элементы совпадают с ожидаемыми для эрозии

    // Освобождение памяти
    freeMatrix(matrix, N);

    // Теперь проверим для наращивания

    // Создаем новую матрицу для наращивания
    matrix = createMatrix(N, 1);

    isErosion = false;  // Для наращивания

    // Применяем фильтры для наращивания
    applyFilters(pool, matrix, N, K, isErosion);

    // Ожидаемая матрица после наращивания
    float expectedDilation[5][5] = {
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2}
    };

    // Сравниваем полученную матрицу с ожидаемой для наращивания
    isCorrect = true;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(matrix[i][j] - expectedDilation[i][j]) > 1e-6) {
                isCorrect = false;
                printf("Mismatch at (%d, %d): Expected %.2f, got %.2f\n", i, j, expectedDilation[i][j], matrix[i][j]);
            }
        }
    }

    ASSERT_TRUE(isCorrect);  // Проверяем, что все элементы совпадают с ожидаемыми для наращивания

    // Освобождение памяти
    freeMatrix(matrix, N);
    destroyThreadPool(pool);  // Освобождаем ресурсы пула потоков
}
