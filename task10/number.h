#ifndef NUMBER_H
#define NUMBER_H

#include "status.h"

Status read_base(const char *text, int *base);
Status read_number(const char *text, const int base, long long *number);
Status add_numbers(const long long left, const long long right, long long *sum);
int abs_greater(const long long left, const long long right);
Status to_base(const long long number, const int base, char *text, const int size);

#endif
