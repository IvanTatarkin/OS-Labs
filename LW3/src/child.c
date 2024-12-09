#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"

const size_t SHARED_MEM_SIZE = 4096;

int main() {
    // Открываем общую память
    int fd = shm_open("/shared_mem", O_RDWR, 0666);
    if (fd == -1) {
        perror("Ошибка открытия общей памяти");
        return 1;
    }

    // Отображаем память с правами только для чтения
    char* shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("Ошибка отображения памяти");
        close(fd);
        return 1;
    }

    // Диагностика: вывод содержимого shared_mem
    printf("Child process started.\n");
    printf("Содержимое shared_mem: %s\n", shared_mem);

    // Создаем локальную копию содержимого общей памяти
    char local_copy[SHARED_MEM_SIZE];
    strncpy(local_copy, shared_mem, SHARED_MEM_SIZE);

    // Извлекаем имя файла и строку чисел из локальной копии
    char *saveptr;
    char* filename = strtok_r(local_copy, "\n", &saveptr);
    char* input = strtok_r(NULL, "\n", &saveptr);

    if (!filename || !input) {
        fprintf(stderr, "Ошибка чтения данных из памяти\n");
        munmap(shared_mem, SHARED_MEM_SIZE);
        close(fd);
        return 1;
    }

    // Диагностика: проверяем извлеченные данные
    printf("Имя файла: %s, Данные: %s\n", filename, input);

    // Открываем файл для записи
    FILE* file = fopen(filename, "a");
    if (!file) {
        perror("Ошибка открытия файла");
        munmap(shared_mem, SHARED_MEM_SIZE);
        close(fd);
        return 1;
    }

    // Диагностика: проверяем перед записью
    printf("Попытка записи в файл: %s\n", filename);

    // Вычисляем сумму чисел
    int sum = calculate_sum(input);
    
    // Диагностика: вывод суммы
    printf("Входные данные для calculate_sum: %s\n", input);
    printf("Результат calculate_sum: %d\n", sum);

    // Записываем результат в файл
    fprintf(file, "Сумма: %d\n", sum);
    fclose(file);

    // Диагностика: успешное завершение работы
    printf("Дочерний процесс завершен, сумма записана в %s.\n", filename);

    // Закрываем и освобождаем память
    munmap(shared_mem, SHARED_MEM_SIZE);
    close(fd);

    return 0;
}
