#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s <имя файла>\n", argv[0]);
        return 1;
    }
    
    char *filename = argv[1];
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Ошибка при открытии файла");
        return 1;
    }
    
    char input[100];
    fgets(input, 100, stdin);
    
    int sum = calculate_sum(input);
    
    fprintf(file, "Сумма: %d\n", sum);
    fflush(file);
    fclose(file);
    
    printf("Дочерний процесс завершен, сумма записана в %s.\n", filename);
    return 0;
}
