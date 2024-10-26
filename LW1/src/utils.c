#include "utils.h"
#include <stdlib.h>
#include <string.h>

int calculate_sum(const char *input) {
    int sum = 0;
    int num;

    char *input_copy = strdup(input);
    if (input_copy == NULL) {
        return 0; 
    }

    char *token = strtok(input_copy, " ");
    while (token != NULL) {
        num = atoi(token); 
        sum += num;         
        token = strtok(NULL, " ");
    }

    free(input_copy);
    return sum;
}
