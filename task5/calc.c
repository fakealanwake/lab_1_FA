#include "calc.h"

#include <math.h>
#include <stddef.h>

// good_eps принимает положительное конечное число погрешности
static int good_eps(const double eps) {
    return isfinite(eps) && eps > 0.0;
}

// ---------------------------------------------

// row_a считает сумму x^n / n! от n = 0 до бесконечности.
// Это ряд для e^x. Следующий член получается умножением предыдущего на x / n.
Status row_a(const double x, const double eps, double *result) {
    double sum = 1.0;
    double term = 1.0;
    int n;

    if (!good_eps(eps) || result == NULL || !isfinite(x)) {
        return BAD_INPUT;
    }
    for (n = 1; n < 1000000; n++) {
        term *= x / (double)n;
        sum += term;
        // ряд оборван, когда очередной член стал меньше заданной точности
        if (fabs(term) < eps) {
            *result = sum;
            return OK;
        }
    }
    return NOT_DONE;
}

// row_b считает сумму (-1)^n * x^(2n) / (2n)!.
// Это ряд для cos(x). Знак члена меняется на каждом шаге.
Status row_b(const double x, const double eps, double *result) {
    double sum = 1.0;
    double term = 1.0;
    double square = x * x;
    int n;

    if (!good_eps(eps) || result == NULL || !isfinite(x)) {
        return BAD_INPUT;
    }
    for (n = 1; n < 1000000; n++) {
        term *= -square / ((2.0 * (double)n - 1.0) * (2.0 * (double)n));
        sum += term;
        if (fabs(term) < eps) {
            *result = sum;
            return OK;
        }
    }
    return NOT_DONE;
}

// row_c считает сумму 3^(3n) * (n!)^3 * x^(2n) / (3n)!.
// Ряд сходится только при |x| < 1.
Status row_c(const double x, const double eps, double *result) {
    double sum = 1.0;
    double term = 1.0;
    double square;
    int n;

    if (!good_eps(eps) || result == NULL || !isfinite(x) || fabs(x) >= 1.0) {
        return BAD_INPUT;
    }
    square = x * x;
    for (n = 1; n < 1000000; n++) {
        term *= 27.0 * (double)n * (double)n * (double)n * square /
                ((3.0 * (double)n - 2.0) * (3.0 * (double)n - 1.0) * (3.0 * (double)n));
        sum += term;
        if (fabs(term) < eps) {
            *result = sum;
            return OK;
        }
    }
    return NOT_DONE;
}

// row_d считает сумму (-1)^n * (2n-1)!! * x^(2n) / (2n)!! начиная с n = 1.
// Первый член равен -x^2 / 2. Тоже сходится только при |x| < 1.
Status row_d(const double x, const double eps, double *result) {
    double square = x * x;
    double term = -square / 2.0;
    double sum = 0.0;
    int n;

    if (!good_eps(eps) || result == NULL || !isfinite(x) || fabs(x) >= 1.0) {
        return BAD_INPUT;
    }
    for (n = 1; n < 1000000; n++) {
        sum += term;
        if (fabs(term) < eps) {
            *result = sum;
            return OK;
        }
        term *= -(2.0 * (double)n + 1.0) * square / (2.0 * (double)n + 2.0);
    }
    return NOT_DONE;
}

// ---------------------------------------------

// f_log_over_x — подынтегральная функция ln(1+x) / x.
// В нуле она не определена, но предел равен 1, его и подставляем.
static double f_log_over_x(const double x) {
    if (fabs(x) < 1e-12) {
        return 1.0;
    }
    return log(1.0 + x) / x;
}

// f_gauss — подынтегральная функция e^(-x^2 / 2)
static double f_gauss(const double x) {
    return exp(-x * x / 2.0);
}

// f_log_gap — подынтегральная функция ln(1 / (1-x)). В точке x = 1 она уходит в бесконечность.
static double f_log_gap(const double x) {
    return log(1.0 / (1.0 - x));
}

// f_power — подынтегральная функция x^x. В нуле предел равен 1.
static double f_power(const double x) {
    if (fabs(x) < 1e-12) {
        return 1.0;
    }
    return pow(x, x);
}

// integrate считает интеграл методом средних прямоугольников.
// Число отрезков удваивается, пока два соседних ответа не разойдутся меньше чем на eps.
static Status integrate(double (*function)(double), const double left, const double right, const double eps, double *result) {
    int parts = 2;
    int round;
    double previous = 0.0;
    double current = 0.0;

    if (function == NULL || result == NULL || !good_eps(eps) || !(left < right)) {
        return BAD_INPUT;
    }
    for (round = 0; round < 24; round++) {
        double width = (right - left) / (double)parts;
        double sum = 0.0;
        int i;
        for (i = 0; i < parts; i++) {
            sum += function(left + ((double)i + 0.5) * width);
        }
        current = sum * width;
        if (round > 0 && fabs(current - previous) <= eps) {
            *result = current;
            return OK;
        }
        previous = current;
        parts *= 2;
    }
    *result = current;
    return NOT_DONE;
}

// integral_a: интеграл ln(1+x) / x от 0 до 1
Status integral_a(const double eps, double *result) {
    return integrate(f_log_over_x, 0.0, 1.0, eps, result);
}

// integral_b: интеграл e^(-x^2 / 2) от 0 до 1
Status integral_b(const double eps, double *result) {
    return integrate(f_gauss, 0.0, 1.0, eps, result);
}

// integral_c: интеграл ln(1 / (1-x)) от 0 до 1.
// В точке 1 функция бесконечна, поэтому верхний предел чуть меньше 1.
// delta подбирается так, чтобы отрезанный хвост был мал по сравнению с eps.
Status integral_c(const double eps, double *result) {
    double delta = 0.5;
    double upper;
    if (!good_eps(eps) || result == NULL) {
        return BAD_INPUT;
    }
    while (delta > 1e-14 && delta * (1.0 - log(delta)) > eps / 20.0) {
        delta /= 2.0;
    }
    upper = 1.0 - delta;
    return integrate(f_log_gap, 0.0, upper, eps / 5.0, result);
}

// integral_d: интеграл x^x от 0 до 1
Status integral_d(const double eps, double *result) {
    return integrate(f_power, 0.0, 1.0, eps, result);
}
