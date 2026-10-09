#ifndef FUNC_H
#define FUNC_H

#include "status.h"

#include <stdarg.h>

Status is_convex(const double eps, int *answer, const int count, ...);
Status polynomial(double *result, const double x, const int degree, ...);
Status kaprekar_list(unsigned long long *found, int *found_count, const int capacity, const int base, const int count, ...);
Status geometric_mean(double *result, const int count, ...);
Status fast_power(double *result, const double base, const int exponent);
Status find_root(double *root, const double left, const double right, const double eps, double (*function)(double));

#endif
