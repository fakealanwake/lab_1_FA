#ifndef ARRAY_H
#define ARRAY_H

#include "status.h"

Status fill_array(int *array, const int length, const int left, const int right);
Status swap_min_max(int *array, const int length);
Status nearest_sum(const int *a, const int a_len, const int *b, const int b_len, int *c);

#endif
