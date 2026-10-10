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

// compare_int задаёт порядок для qsort: по возрастанию
static int compare_int(const void *left, const void *right) {
    int first = *(const int *)left;
    int second = *(const int *)right;

    if (first < second) {
        return -1;
    }
    if (first > second) {
        return 1;
    }
    return 0;
}

// lower_bound возвращает индекс первого элемента, который не меньше target
static int lower_bound(const int *array, const int length, const int target) {
    int left = 0;
    int right = length;

    while (left < right) {
        int mid = left + (right - left) / 2;
        if (array[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

// abs_diff считает модуль разности без переполнения int
static long long abs_diff(const int left, const int right) {
    long long diff = (long long)left - (long long)right;
    if (diff < 0) {
        diff = -diff;
    }
    return diff;
}

// nearest_sum строит массив C: C[i] = A[i] + ближайший к A[i] элемент из B.
// B копируется и сортируется, ближайший ищется двоичным поиском.
// Если ближайших несколько, берётся любой из них.
Status nearest_sum(const int *a, const int a_len, const int *b, const int b_len, int *c) {
    int *sorted;
    int i;

    if (a == NULL || b == NULL || c == NULL || a_len <= 0 || b_len <= 0) {
        return BAD_INPUT;
    }
    sorted = (int *)malloc((size_t)b_len * sizeof(int));
    if (sorted == NULL) {
        return NO_MEMORY;
    }
    for (i = 0; i < b_len; i++) {
        sorted[i] = b[i];
    }
    qsort(sorted, (size_t)b_len, sizeof(int), compare_int);
    for (i = 0; i < a_len; i++) {
        int pos = lower_bound(sorted, b_len, a[i]);
        int best = sorted[pos < b_len ? pos : b_len - 1];
        long long best_dist = abs_diff(a[i], best);

        if (pos > 0) {
            long long left_dist = abs_diff(a[i], sorted[pos - 1]);
            if (left_dist <= best_dist) {
                best = sorted[pos - 1];
            }
        }
        c[i] = a[i] + best;
    }
    free(sorted);
    return OK;
}
