#ifndef CALC_H
#define CALC_H

#include "status.h"

Status row_a(const double x, const double eps, double *result);
Status row_b(const double x, const double eps, double *result);
Status row_c(const double x, const double eps, double *result);
Status row_d(const double x, const double eps, double *result);
Status integral_a(const double eps, double *result);
Status integral_b(const double eps, double *result);
Status integral_c(const double eps, double *result);
Status integral_d(const double eps, double *result);

#endif
