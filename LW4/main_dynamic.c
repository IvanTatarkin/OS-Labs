#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

int is_number(const char* str) {
    int dot_count = 0;
    int sign_count = 0;
    const char* p = str;
    if (*p == '+' || *p == '-') {
        sign_count++;
        p++;
    }
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
    while (*p) {
        if (!isdigit((unsigned char)*p)) return 0;
        p++;
    }
    return 1;
}

int main() {
    void* handle = NULL; 
    float (*SinIntegral)(float, float, float) = NULL;
    void (*Sort)(int*, int) = NULL;

    
    handle = dlopen("./liblib1.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }

    
    SinIntegral = (float (*)(float,float,float))(uintptr_t)dlsym(handle, "SinIntegral_rectangle");
    if (!SinIntegral) {
        fprintf(stderr, "Ошибка поиска SinIntegral_rectangle: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }
    Sort = (void (*)(int*,int))(uintptr_t)dlsym(handle, "BubbleSort");
    if (!Sort) {
        fprintf(stderr, "Ошибка поиска BubbleSort: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    char input[256];
    printf("Режимы:\n"
           "0 - Смена реализаций (между lib1 и lib2),\n"
           "1 - Расчет интеграла sin(x),\n"
           "2 - Сортировка массива,\n"
           "3 - Выход.\n");

    int current_lib = 1; 
    while (1) {
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

        if (choice == 0) {
            
            dlclose(handle);
            if (current_lib == 1) {
                handle = dlopen("./liblib2.so", RTLD_LAZY);
                current_lib = 2;
            } else {
                handle = dlopen("./liblib1.so", RTLD_LAZY);
                current_lib = 1;
            }

            if (!handle) {
                fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
                return 1;
            }

            if (current_lib == 1) {
                SinIntegral = (float (*)(float,float,float))(uintptr_t)dlsym(handle, "SinIntegral_rectangle");
                Sort = (void (*)(int*,int))(uintptr_t)dlsym(handle, "BubbleSort");
            } else {
                SinIntegral = (float (*)(float,float,float))(uintptr_t)dlsym(handle, "SinIntegral_trapezoid");
                Sort = (void (*)(int*,int))(uintptr_t)dlsym(handle, "QuickSort");
            }

            if (!SinIntegral || !Sort) {
                fprintf(stderr, "Ошибка поиска функций: %s\n", dlerror());
                dlclose(handle);
                return 1;
            }

            printf("Библиотека переключена.\n");
        } else if (choice == 1) {
            
            
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
            float result = SinIntegral(A, B, e);
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

            printf("Введите %d целых чисел: ", size);
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

            
            Sort(array, size);

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
            printf("Некорректный ввод\n");
        }
    }

    dlclose(handle);
    return 0;
}
