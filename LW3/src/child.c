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
    int fd = shm_open("/shared_mem", O_RDWR, 0666);
    if (fd == -1) {
        perror("Ошибка открытия общей памяти");
        return 1;
    }

    char* shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("Ошибка отображения памяти");
        close(fd);
        return 1;
    }

    printf("Child process started.\n");
    printf("Содержимое shared_mem:\n%s\n", shared_mem);

    char local_copy[SHARED_MEM_SIZE];
    strncpy(local_copy, shared_mem, SHARED_MEM_SIZE);

    char *saveptr;
    
    char* filename = strtok_r(local_copy, "\n", &saveptr);

    if (!filename) {
        fprintf(stderr, "Ошибка чтения имени файла\n");
        munmap(shared_mem, SHARED_MEM_SIZE);
        close(fd);
        return 1;
    }

    FILE* file = fopen(filename, "a");
    if (!file) {
        perror("Ошибка открытия файла");
        munmap(shared_mem, SHARED_MEM_SIZE);
        close(fd);
        return 1;
    }

    printf("Обработка файла: %s\n", filename);

    
    char* line = NULL;
    while ((line = strtok_r(NULL, "\n", &saveptr)) != NULL) {
        printf("Входные данные для calculate_sum: %s\n", line);
        int sum = calculate_sum(line);
        printf("Результат calculate_sum: %d\n", sum);

        fprintf(file, "Сумма: %d\n", sum);
    }

    fclose(file);

    printf("Дочерний процесс завершен, результаты записаны в %s.\n", filename);

    munmap(shared_mem, SHARED_MEM_SIZE);
    close(fd);

    return 0;
}
