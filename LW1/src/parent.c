#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "parent.h"
#include "utils.h"

const int FILENAME_SIZE = 100;

void Parent(const char* pathToChild, FILE* stream) {
    int pipefd[2];
    pid_t pid;
    char filename[FILENAME_SIZE];
    
    printf("Введите имя файла: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        perror("Ошибка ввода имени файла");
        return;
    }
    filename[strcspn(filename, "\n")] = 0;
    
    if (pipe(pipefd) == -1) {
        perror("Ошибка создания pipe");
        return;
    }

    pid = fork();
    if (pid < 0) {
        perror("Ошибка при fork");
        return;
    }
    
    if (pid == 0) { 
        close(pipefd[1]); 
        dup2(pipefd[0], STDIN_FILENO);
        execl(pathToChild, "child", filename, NULL);
        perror("Ошибка при execl");
        exit(1);
    } else { 
        close(pipefd[0]); 
        
        char input[100];
        printf("Введите числа через пробел (закончите ввод нажатием Enter): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Ошибка ввода чисел");
            return;
        }
        
        write(pipefd[1], input, strlen(input) + 1); 
        close(pipefd[1]); 
        
        wait(NULL);
        printf("Родительский процесс завершён.\n");
    }
}
