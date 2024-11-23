#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>

// Глобальные переменные
pthread_mutex_t mutex;
int maxCountOfThreads = 4;  // Количество потоков по умолчанию

// Структура для передачи данных потока
typedef struct {
    float **matrix;      // Матрица для обработки
    int rows;            // Количество строк в матрице
    int cols;            // Количество столбцов в матрице
    int iterations;      // Количество итераций применения фильтра
    int startRow;        // Начальная строка для обработки
    int startCol;        // Начальный столбец для обработки
    int endRow;          // Конечная строка для обработки
    int endCol;          // Конечный столбец для обработки
    bool isDilation;     // Тип фильтра (0 - эрозия, 1 - наращивание)
} FilterData;

// Функции для генерации и освобождения матрицы
float **generateMatrix(int rows, int cols) {
    float **matrix = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; ++i) {
        matrix[i] = (float *)malloc(cols * sizeof(float));
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = (rand() % 100) / 10.0;  // Случайные значения от 0 до 9.9
        }
    }
    return matrix;
}

void freeMatrix(float **matrix, int rows) {
    for (int i = 0; i < rows; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

// Функции для сравнения значений с плавающей точкой
bool areEqualWithTolerance(float a, float b, float tolerance) {
    return fabs(a - b) < tolerance;
}

void debugMismatch(float expected, float actual, int row, int col) {
    printf("Mismatch at (%d, %d): expected %f, got %f\n", row, col, expected, actual);
}

// Функции для применения фильтров (эрозия и наращивание)
void applyErosion(FilterData *data) {
    float **tempMatrix = (float **)malloc(data->rows * sizeof(float *));
    for (int i = 0; i < data->rows; ++i) {
        tempMatrix[i] = (float *)malloc(data->cols * sizeof(float));
    }

    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = data->startCol; j < data->endCol; j++) {
            if (i > 0 && j > 0 && i < data->rows - 1 && j < data->cols - 1) {
                float minVal = data->matrix[i][j];
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < data->rows && nj >= 0 && nj < data->cols) {
                            if (data->matrix[ni][nj] < minVal) {
                                minVal = data->matrix[ni][nj];
                            }
                        }
                    }
                }
                tempMatrix[i][j] = minVal;
            }
        }
    }

    for (int i = data->startRow; i < data->endRow; ++i) {
        for (int j = data->startCol; j < data->endCol; ++j) {
            data->matrix[i][j] = tempMatrix[i][j];
        }
    }

    for (int i = 0; i < data->rows; ++i) {
        free(tempMatrix[i]);
    }
    free(tempMatrix);
}

void applyDilation(FilterData *data) {
    float **tempMatrix = (float **)malloc(data->rows * sizeof(float *));
    for (int i = 0; i < data->rows; ++i) {
        tempMatrix[i] = (float *)malloc(data->cols * sizeof(float));
    }

    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = data->startCol; j < data->endCol; j++) {
            if (i > 0 && j > 0 && i < data->rows - 1 && j < data->cols - 1) {
                float maxVal = data->matrix[i][j];
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < data->rows && nj >= 0 && nj < data->cols) {
                            if (data->matrix[ni][nj] > maxVal) {
                                maxVal = data->matrix[ni][nj];
                            }
                        }
                    }
                }
                tempMatrix[i][j] = maxVal;
            }
        }
    }

    for (int i = data->startRow; i < data->endRow; ++i) {
        for (int j = data->startCol; j < data->endCol; ++j) {
            data->matrix[i][j] = tempMatrix[i][j];
        }
    }

    for (int i = 0; i < data->rows; ++i) {
        free(tempMatrix[i]);
    }
    free(tempMatrix);
}

// Общая функция для применения фильтров (эрозия или наращивание)
void *applyFilterThread(void *arg) {
    FilterData *data = (FilterData *)arg;
    for (int i = 0; i < data->iterations; i++) {
        if (data->isDilation) {
            applyDilation(data);
        } else {
            applyErosion(data);
        }
    }

    return NULL;
}

// Функция для параллельного применения фильтров
void applyFiltersParallel(FilterData *data) {
    pthread_t threads[maxCountOfThreads];
    FilterData threadData[maxCountOfThreads];
    int cellsPerThread = (data->rows * data->cols) / maxCountOfThreads;
    int extraCells = (data->rows * data->cols) % maxCountOfThreads;

    int startCell = 0;
    for (int i = 0; i < maxCountOfThreads; ++i) {
        int endCell = startCell + cellsPerThread + (i == maxCountOfThreads - 1 ? extraCells : 0);
        
        // Распределяем каждый поток на обработку сегментов 3x3
        threadData[i] = *data;
        threadData[i].startRow = startCell / data->cols;
        threadData[i].startCol = startCell % data->cols;
        threadData[i].endRow = endCell / data->cols;
        threadData[i].endCol = endCell % data->cols;

        pthread_create(&threads[i], NULL, applyFilterThread, (void *)&threadData[i]);
        startCell = endCell;
    }

    // Ожидаем завершения всех потоков
    for (int i = 0; i < maxCountOfThreads; ++i) {
        pthread_join(threads[i], NULL);
    }
}

// Функция для применения фильтров к матрице в одном потоке (для сравнения)
void applyFiltersSingleThread(FilterData *data) {
    applyErosion(data);  // Применяем эрозию
    applyDilation(data); // Применяем наращивание
}

// Функция для печати матрицы
void printMatrix(float **matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%6.2f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Функция для копирования матрицы
float **copyMatrix(float **matrix, int rows, int cols) {
    float **copy = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; ++i) {
        copy[i] = (float *)malloc(cols * sizeof(float));
        for (int j = 0; j < cols; ++j) {
            copy[i][j] = matrix[i][j];
        }
    }
    return copy;
}

int main() {
    int matrixSize, iterations, threadsCount;
    
    // Ввод данных пользователем
    printf("Введите максимальное количество потоков: ");
    scanf("%d", &threadsCount);
    maxCountOfThreads = threadsCount;

    printf("Введите размер матрицы (например, 5 для матрицы 5x5): ");
    scanf("%d", &matrixSize);

    printf("Введите количество итераций фильтра: ");
    scanf("%d", &iterations);

    // Создание и инициализация случайной матрицы
    float **matrix = generateMatrix(matrixSize, matrixSize);
    printf("Исходная матрица:\n");
    printMatrix(matrix, matrixSize, matrixSize);  // Вывод исходной матрицы

    float **matrixCopy = copyMatrix(matrix, matrixSize, matrixSize);

    // Устанавливаем значение фильтра
    FilterData data;
    data.matrix = matrix;
    data.rows = matrixSize;
    data.cols = matrixSize;
    data.iterations = iterations;
    data.isDilation = true;  // Дилация, если true, эрозия - если false

    pthread_mutex_init(&mutex, NULL);

    // Применяем фильтры с использованием нескольких потоков
    applyFiltersParallel(&data);
    printf("Матрица после наращивания (многопоточно):\n");
    printMatrix(matrix, matrixSize, matrixSize);  // Вывод матрицы после применения фильтра с многопоточностью

    // Применяем фильтры с одним потоком для проверки результатов
    data.matrix = matrixCopy;  // Используем копию
    applyFiltersSingleThread(&data);
    printf("Матрица после наращивания (одним потоком):\n");
    printMatrix(matrixCopy, matrixSize, matrixSize);  // Вывод матрицы после применения фильтра с одним потоком

    // Сравнение результатов
    bool resultsMatch = true;
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            if (!areEqualWithTolerance(matrix[i][j], matrixCopy[i][j], 0.01)) {
                debugMismatch(matrixCopy[i][j], matrix[i][j], i, j);
                resultsMatch = false;
            }
        }
    }

    if (resultsMatch) {
        printf("Результаты совпадают!\n");
    } else {
        printf("Найдены расхождения!\n");
    }

    // Освобождение памяти
    freeMatrix(matrix, matrixSize);
    freeMatrix(matrixCopy, matrixSize);

    pthread_mutex_destroy(&mutex);
    return 0;
}
