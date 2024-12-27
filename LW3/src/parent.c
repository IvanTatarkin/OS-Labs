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

    while (1) {
        char filename[100];
        printf("Введите имя файла (или введите 'exit' для завершения): ");
        if (fgets(filename, sizeof(filename), stdin) == NULL) {
            perror("Ошибка ввода имени файла");
            break;
        }
        filename[strcspn(filename, "\n")] = 0;

        if (strcmp(filename, "exit") == 0) {
            printf("Выход из программы.\n");
            break;
        }

        
        memset(shared_mem, 0, SHARED_MEM_SIZE);

        
        snprintf(shared_mem, SHARED_MEM_SIZE, "%s", filename);

        
        size_t offset = strlen(filename);
        shared_mem[offset++] = '\n';

        printf("Теперь введите несколько групп чисел. Введите 'done', чтобы завершить ввод и выполнить обработку.\n");

        while (1) {
            char input[100];
            printf("Введите группу чисел через пробел (или 'done' для обработки): ");
            if (fgets(input, sizeof(input), stdin) == NULL) {
                perror("Ошибка ввода чисел");
                break;
            }
            input[strcspn(input, "\n")] = 0;

            if (strcmp(input, "done") == 0) {
                break;
            }

            
            size_t input_len = strlen(input);
            if (offset + input_len + 1 >= SHARED_MEM_SIZE) {
                fprintf(stderr, "Превышен размер общей памяти. Выполните 'done' для обработки.\n");
                continue;
            }

            
            memcpy(shared_mem + offset, input, input_len);
            offset += input_len;
            shared_mem[offset++] = '\n';
        }

        
        pid_t pid = fork();
        if (pid < 0) {
            perror("Ошибка при fork");
            break;
        }

        if (pid == 0) {
            execl(pathToChild, "child", NULL);
            perror("Ошибка при execl");
            exit(1);
        } else {
            wait(NULL);
            printf("Родительский процесс завершён обработку для файла '%s'. Вы можете ввести новые данные.\n", filename);
        }
    }

    munmap(shared_mem, SHARED_MEM_SIZE);
    shm_unlink("/shared_mem");
}
