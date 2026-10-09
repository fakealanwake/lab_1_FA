#include "array.h"

#include <stdlib.h>

// fill_array заполняет массив псевдослучайными целыми от left до right включительно
Status fill_array(int *array, const int length, const int left, const int right) {
    long long span;
    int i;

    if (array == NULL || length <= 0 || left > right) {
        return BAD_INPUT;
    }
    // сколько разных значений можно получить: от left до right включительно
    span = (long long)right - (long long)left + 1LL;
    for (i = 0; i < length; i++) {
        unsigned long long rnd = (unsigned long long)rand() * 32768ULL + (unsigned long long)rand();
        array[i] = (int)((long long)left + (long long)(rnd % (unsigned long long)span));
    }
    return OK;
}

// ---------------------------------------------

// swap_min_max за один проход находит индексы минимума и максимума и меняет эти элементы местами
Status swap_min_max(int *array, const int length) {
    int i;
    int min_index = 0;
    int max_index = 0;
    int tmp;

    if (array == NULL || length <= 0) {
        return BAD_INPUT;
    }
    for (i = 1; i < length; i++) {
        if (array[i] < array[min_index]) {
            min_index = i;
        }
        if (array[i] > array[max_index]) {
            max_index = i;
        }
    }
    // обмен найденных элементов. Если минимум и максимум — один и тот же элемент, массив не меняется.
    tmp = array[min_index];
    array[min_index] = array[max_index];
    array[max_index] = tmp;
    return OK;
}

// ---------------------------------------------

// nearest_sum строит массив C: C[i] = A[i] + ближайший к A[i] элемент из B.
// Если ближайших несколько, берётся любой из них — здесь первый найденный.
Status nearest_sum(const int *a, const int a_len, const int *b, const int b_len, int *c) {
    int i;

    if (a == NULL || b == NULL || c == NULL || a_len <= 0 || b_len <= 0) {
        return BAD_INPUT;
    }
    for (i = 0; i < a_len; i++) {
        int j;
        int best = b[0];
        long long best_dist = (long long)a[i] - (long long)b[0];
        if (best_dist < 0) {
            best_dist = -best_dist;
        }
        for (j = 1; j < b_len; j++) {
            long long dist = (long long)a[i] - (long long)b[j];
            if (dist < 0) {
                dist = -dist;
            }
            // строгое < оставляет первый из равных по расстоянию элементов
            if (dist < best_dist) {
                best_dist = dist;
                best = b[j];
            }
        }
        c[i] = a[i] + best;
    }
    return OK;
}
