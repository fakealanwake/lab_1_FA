#ifndef CALC_H
#define CALC_H

#include "status.h"

Status exp_by_series(const double eps, double *result);
Status exp_by_limit(const double eps, double *result);
Status exp_by_root(const double eps, double *result);
Status pi_by_series(const double eps, double *result);
Status pi_by_product(const double eps, double *result);
Status pi_by_root(const double eps, double *result);
Status ln2_by_series(const double eps, double *result);
Status ln2_by_limit(const double eps, double *result);
Status ln2_by_root(const double eps, double *result);
Status sqrt2_by_newton(const double eps, double *result);
Status sqrt2_by_product(const double eps, double *result);
Status sqrt2_by_root(const double eps, double *result);
Status gamma_by_series(const double eps, double *result);
Status gamma_by_limit(const double eps, double *result);
Status gamma_by_primes(const double eps, double *result);

#endif
