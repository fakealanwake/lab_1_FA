#include "calc.h"
#include <math.h>
#include <stddef.h>

// good_eps принимает положительное конечное число погрешности
static int good_eps(const double eps) {
    return isfinite(eps) && eps > 0.0;
}

// prime_number проверяет, простое ли целое. Нужно для произведения по простым в формуле для γ.
static int prime_number(const int number) {
    int divisor;
    if (number < 2) {
        return 0;
    }
    if (number % 2 == 0) {
        return number == 2;
    }
    for (divisor = 3; divisor * divisor <= number; divisor += 2) {
        if (number % divisor == 0) {
            return 0;
        }
    }
    return 1;
}

// ---------------------------------------------

// exp_by_series считает e как сумму 1/n!.
// Очередной член равен предыдущему, делённому на n. Останавливаемся, когда член меньше eps.
Status exp_by_series(const double eps, double *result) {
    double sum = 1.0;
    double term = 1.0;
    int step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 1; step < 1000; step++) {
        term /= step;
        sum += term;
        if (term < eps) {
            *result = sum;
            return OK;
        }
    }
    *result = sum;
    return NOT_DONE;
}

// exp_by_limit считает e как предел (1 + 1/n)^n при большом n
Status exp_by_limit(const double eps, double *result) {
    double steps;
    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    steps = 2.0 / eps;
    if (steps > 1e7) {
        steps = 1e7;
    }
    *result = pow(1.0 + 1.0 / steps, steps);
    return (2.0 / steps) <= eps ? OK : NOT_DONE;
}

// exp_by_root решает уравнение ln x = 1 делением отрезка [2, 3] пополам.
// Если ln(середины) меньше 1, корень правее.
Status exp_by_root(const double eps, double *result) {
    double left = 2.0;
    double right = 3.0;
    int step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 0; step < 200 && right - left > eps; step++) {
        double middle = (left + right) / 2.0;
        if (log(middle) < 1.0) {
            left = middle;
        } else {
            right = middle;
        }
    }
    *result = (left + right) / 2.0;
    return right - left <= eps ? OK : NOT_DONE;
}

// ---------------------------------------------

// pi_by_series считает π рядом Лейбница: 4 * (1 - 1/3 + 1/5 - 1/7 + ...)
Status pi_by_series(const double eps, double *result) {
    double sum = 0.0;
    double sign = 1.0;
    long step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 0; step < 2000000L; step++) {
        double term = sign / (2.0 * step + 1.0);
        sum += term;
        sign = -sign;
        if (fabs(4.0 * term) < eps) {
            *result = 4.0 * sum;
            return OK;
        }
    }
    *result = 4.0 * sum;
    return NOT_DONE;
}

// pi_by_product считает π бесконечным произведением: 2 * произведение 4n^2 / (4n^2 - 1)
Status pi_by_product(const double eps, double *result) {
    double product = 1.0;
    long step;
    long limit;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    limit = (long)(4.0 / eps);
    if (limit < 1) {
        limit = 1;
    }
    if (limit > 2000000L) {
        limit = 2000000L;
    }
    for (step = 1; step <= limit; step++) {
        product *= (4.0 * step * step) / (4.0 * step * step - 1.0);
    }
    *result = 2.0 * product;
    return (4.0 / eps) <= 2000000.0 ? OK : NOT_DONE;
}

// pi_by_root ищет решение cos x = -1 на отрезке [3, 4].
// Левее этого корня sin x ещё положителен, поэтому середину сдвигаем вправо.
Status pi_by_root(const double eps, double *result) {
    double left = 3.0;
    double right = 4.0;
    int step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 0; step < 200 && right - left > eps; step++) {
        double middle = (left + right) / 2.0;
        if (sin(middle) > 0.0) {
            left = middle;
        } else {
            right = middle;
        }
    }
    *result = (left + right) / 2.0;
    return right - left <= eps ? OK : NOT_DONE;
}

// ---------------------------------------------

// ln2_by_series считает ln 2 рядом 1 - 1/2 + 1/3 - 1/4 + ...
Status ln2_by_series(const double eps, double *result) {
    double sum = 0.0;
    double sign = 1.0;
    long step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 1; step <= 2000000L; step++) {
        double term = sign / (double)step;
        if (fabs(term) < eps) {
            *result = sum;
            return OK;
        }
        sum += term;
        sign = -sign;
    }
    *result = sum;
    return NOT_DONE;
}

// ln2_by_limit считает ln 2 как предел n * (2^(1/n) - 1)
Status ln2_by_limit(const double eps, double *result) {
    double steps;
    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    steps = 1.0 / eps;
    if (steps > 1e7) {
        steps = 1e7;
    }
    *result = steps * (pow(2.0, 1.0 / steps) - 1.0);
    return (1.0 / steps) <= eps ? OK : NOT_DONE;
}

// ln2_by_root решает уравнение e^x = 2 на отрезке [0, 1]
Status ln2_by_root(const double eps, double *result) {
    double left = 0.0;
    double right = 1.0;
    int step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 0; step < 200 && right - left > eps; step++) {
        double middle = (left + right) / 2.0;
        if (exp(middle) < 2.0) {
            left = middle;
        } else {
            right = middle;
        }
    }
    *result = (left + right) / 2.0;
    return right - left <= eps ? OK : NOT_DONE;
}

// ---------------------------------------------

// sqrt2_by_newton приближает корень из 2 повторением x = (x + 2/x) / 2
Status sqrt2_by_newton(const double eps, double *result) {
    double value = 1.0;
    int step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 0; step < 80; step++) {
        value = (value + 2.0 / value) / 2.0;
        if (fabs(value * value - 2.0) < eps) {
            *result = value;
            return OK;
        }
    }
    *result = value;
    return NOT_DONE;
}

// sqrt2_by_product считает корень из 2 произведением 2^(1/4) * 2^(1/8) * 2^(1/16) * ...
Status sqrt2_by_product(const double eps, double *result) {
    double product = 1.0;
    double exponent = 0.25;
    int step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 0; step < 60; step++) {
        double factor = pow(2.0, exponent);
        product *= factor;
        if (exponent * log(2.0) < eps / 3.0) {
            *result = product;
            return OK;
        }
        exponent /= 2.0;
    }
    *result = product;
    return NOT_DONE;
}

// sqrt2_by_root решает уравнение x^2 = 2 на отрезке [1, 2]
Status sqrt2_by_root(const double eps, double *result) {
    double left = 1.0;
    double right = 2.0;
    int step;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (step = 0; step < 200 && right - left > eps; step++) {
        double middle = (left + right) / 2.0;
        if (middle * middle < 2.0) {
            left = middle;
        } else {
            right = middle;
        }
    }
    *result = (left + right) / 2.0;
    return right - left <= eps ? OK : NOT_DONE;
}

// ---------------------------------------------

// gamma_by_series считает постоянную Эйлера γ суммой (1/k - ln(1 + 1/k))
Status gamma_by_series(const double eps, double *result) {
    double sum = 0.0;
    long step;
    long limit;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    limit = (long)(2.0 / eps);
    if (limit < 10) {
        limit = 10;
    }
    if (limit > 2000000L) {
        limit = 2000000L;
    }
    for (step = 1; step <= limit; step++) {
        sum += 1.0 / (double)step - log1p(1.0 / (double)step);
    }
    *result = sum;
    return (1.0 / (2.0 * (double)limit)) <= eps ? OK : NOT_DONE;
}

// gamma_by_limit считает γ как предел H_n - ln n, где H_n — сумма 1 + 1/2 + ... + 1/n
Status gamma_by_limit(const double eps, double *result) {
    double harmonic = 0.0;
    long step;
    long limit;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    limit = (long)(2.0 / eps);
    if (limit < 2) {
        limit = 2;
    }
    if (limit > 2000000L) {
        limit = 2000000L;
    }
    for (step = 1; step <= limit; step++) {
        harmonic += 1.0 / (double)step;
    }
    *result = harmonic - log((double)limit);
    return (1.0 / (2.0 * (double)limit)) <= eps ? OK : NOT_DONE;
}

// gamma_by_primes ищет γ из произведения по простым:
// e^(-γ) приближается выражением ln(t) * произведение (p - 1) / p по простым p <= t
Status gamma_by_primes(const double eps, double *result) {
    double product = 1.0;
    double previous = 0.0;
    double current = 0.0;
    int value;
    int limit = 200000;

    if (!good_eps(eps) || result == NULL) {
        return BAD_ARG;
    }
    for (value = 2; value <= limit; value++) {
        if (prime_number(value)) {
            product *= (value - 1.0) / (double)value;
        }
        if (value > 2) {
            previous = current;
            // γ ≈ -ln( ln(t) * произведение (1 - 1/p) )
            current = -log(log((double)value) * product);
        }
    }
    *result = current;
    if (fabs(current - previous) < eps && 1.0 / log((double)limit) < eps) {
        return OK;
    }
    return NOT_DONE;
}
