#include <math.h>
#include <stdlib.h>
#include <string.h>


float SinIntegral_rectangle(float A, float B, float e) {
    float sum = 0.0;
    for(float x = A; x < B; x += e) {
        sum += sin(x) * e;
    }
    return sum;
}

void BubbleSort(int *array, int size) {
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}