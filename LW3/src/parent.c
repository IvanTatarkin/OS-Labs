#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "parent.h"

const size_t SHARED_MEM_SIZE = 4096;

void Parent(const char* pathToChild, FILE* stream) {
    int fd = shm_open("/shared_mem", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Ошибка создания общей памяти");
        return;
    }

    if (ftruncate(fd, SHARED_MEM_SIZE) == -1) {
        perror("Ошибка установки размера общей памяти");
        return;
    }

    char* shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("Ошибка отображения памяти");
        return;
    }

    char filename[100];
    printf("Введите имя файла: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        perror("Ошибка ввода имени файла");
        return;
    }
    filename[strcspn(filename, "\n")] = 0; // Удаляем символ новой строки

    printf("Введите числа через пробел (закончите ввод нажатием Enter): ");
    char input[100];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("Ошибка ввода чисел");
        return;
    }

    // Форматируем и записываем имя файла и данные в shared_mem
    snprintf(shared_mem, SHARED_MEM_SIZE, "%s\n%s", filename, input);

    pid_t pid = fork();
    if (pid < 0) {
        perror("Ошибка при fork");
        return;
    }

    if (pid == 0) { // Дочерний процесс
        execl(pathToChild, "child", NULL);
        perror("Ошибка при execl");
        exit(1);
    } else { // Родительский процесс
        wait(NULL); // Ждем завершения дочернего процесса
        printf("Родительский процесс завершён.\n");
    }

    munmap(shared_mem, SHARED_MEM_SIZE);
    shm_unlink("/shared_mem");
}
