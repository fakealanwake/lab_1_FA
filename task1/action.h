#ifndef ACTION_H
#define ACTION_H

#include "status.h"

Status collect_multiples(const long long number, long long *list, int *count);
Status classify_number(const long long number, int *kind);
Status format_hex(const long long number, char *text, const int capacity);
Status fill_powers(const long long degree, unsigned long long table[11][11]);
Status sum_naturals(const long long number, unsigned long long *total);
Status factorial(const long long number, unsigned long long *result);

#endif
