#include <stdio.h>
#include <stdlib.h> 
#include <ctype.h>
#include <string.h>
#include "src/lib1.c" 


int is_number(const char* str) {
    int dot_count = 0;
    int sign_count = 0;
    const char* p = str;
    if (*p == '+' || *p == '-') {
        sign_count++;
        p++;
    }
    if (*p == '\0') return 0; 

    while (*p) {
        if (*p == '.') {
            dot_count++;
            if (dot_count > 1) return 0;
        } else if (!isdigit((unsigned char)*p)) {
            return 0;
        }
        p++;
    }
    return 1;
}

int is_integer(const char* str) {
    const char* p = str;
    if (*p == '+' || *p == '-') p++;
    if (*p == '\0') return 0; 
    while (*p) {
        if (!isdigit((unsigned char)*p)) return 0;
        p++;
    }
    return 1;
}

int main() {
    printf("Режимы:\n"
           "1 - Расчет интеграла sin(x) методом прямоугольников,\n"
           "2 - Сортировка массива методом пузырька,\n"
           "3 - Выход.\n");

    while (1) {
        char input[256];
        printf("Введите команду: ");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Ошибка ввода.\n");
            continue;
        }

        char* cmd = strtok(input, " \n");
        if (!cmd) {
            printf("Некорректный ввод.\n");
            continue;
        }

        int choice = atoi(cmd);

        if (choice == 1) {
            
            
            printf("Введите A B e через пробел: ");
            if (!fgets(input, sizeof(input), stdin)) {
                printf("Ошибка ввода.\n");
                continue;
            }

            char* A_str = strtok(input, " \n");
            char* B_str = strtok(NULL, " \n");
            char* e_str = strtok(NULL, " \n");

            if (!A_str || !B_str || !e_str) {
                printf("Ошибка: введите три числа.\n");
                continue;
            }

            if (!is_number(A_str) || !is_number(B_str) || !is_number(e_str)) {
                printf("Ошибка: аргументы должны быть числами.\n");
                continue;
            }

            float A = atof(A_str);
            float B = atof(B_str);
            float e = atof(e_str);

            if (e <= 0) {
                printf("Ошибка: шаг e должен быть > 0.\n");
                continue;
            }

            float result = SinIntegral_rectangle(A, B, e);
            printf("Результат интеграла sin(x) от %f до %f с шагом %f: %f\n", A, B, e, result);

        } else if (choice == 2) {
            
            printf("Введите размер массива: ");
            if (!fgets(input, sizeof(input), stdin)) {
                printf("Ошибка ввода.\n");
                continue;
            }

            char* size_str = strtok(input, " \n");
            if (!size_str || !is_integer(size_str)) {
                printf("Ошибка: размер массива должен быть целым числом.\n");
                continue;
            }

            int size = atoi(size_str);
            if (size <= 0) {
                printf("Ошибка: размер массива должен быть > 0.\n");
                continue;
            }

            int* array = (int*)malloc(size * sizeof(int));
            if (!array) {
                printf("Ошибка выделения памяти.\n");
                continue;
            }

            printf("Введите %d целых чисел через пробел: ", size);
            int count = 0;
            for (int i = 0; i < size; i++) {
                if (scanf("%d", &array[i]) != 1) {
                    printf("Ошибка ввода элемента.\n");
                    count = i;
                    break;
                }
                count++;
            }
            
            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            if (count < size) {
                free(array);
                continue;
            }

            
            BubbleSort(array, size);

            printf("Отсортированный массив: ");
            for (int i = 0; i < size; i++) {
                printf("%d ", array[i]);
            }
            printf("\n");
            free(array);

        } else if (choice == 3) {
            printf("Завершение работы\n");
            break;
        } else {
            printf("Некорректный ввод. Попробуйте ещё раз.\n");
        }
    }

    return 0;
}
