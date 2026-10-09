#ifndef SOLVE_H
#define SOLVE_H

#include "status.h"

typedef struct Equation {
    double first;
    double second;
    double third;
    double root1;
    double root2;
    int root_count;
} Equation;

Status solve_all_orders(const double eps, const double a, const double b, const double c, Equation *list, int *count);
Status is_multiple(const long long left, const long long right, int *answer);
Status is_right_triangle(const double eps, const double a, const double b, const double c, int *answer);

#endif
