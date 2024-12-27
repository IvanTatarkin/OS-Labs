#include <stdlib.h>
#include <string.h>
#include <math.h>

float SinIntegral_trapezoid(float A, float B, float e) {
    float sum = 0.0;
    for(float x = A; x < B; x += e) {
        sum += (sin(x) + sin(x + e)) / 2 * e;
    }
    return sum;
}

void QuickSortRecursive(int *array, int left, int right) {
    if (left < right) {
        int pivot = array[right];
        int i = left - 1;

        for (int j = left; j < right; ++j) {
            if (array[j] <= pivot) {
                ++i;
                int temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }

        int temp = array[i + 1];
        array[i + 1] = array[right];
        array[right] = temp;

        QuickSortRecursive(array, left, i);
        QuickSortRecursive(array, i + 2, right);
    }
}

void QuickSort(int *array, int size) { QuickSortRecursive(array, 0, size - 1); }