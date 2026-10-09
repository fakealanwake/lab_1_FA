#include "func.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>

typedef struct Point {
    double x;
    double y;
} Point;

// good_eps принимает положительное конечное число погрешности
static int good_eps(const double eps) {
    return isfinite(eps) && eps > 0.0;
}

// digit_value переводит символ цифры в число: '0'..'9' дают 0..9, 'A' и 'a' дают 10
static int digit_value(const char symbol) {
    if (symbol >= '0' && symbol <= '9') {
        return symbol - '0';
    }
    if (symbol >= 'A' && symbol <= 'Z') {
        return symbol - 'A' + 10;
    }
    if (symbol >= 'a' && symbol <= 'z') {
        return symbol - 'a' + 10;
    }
    return -1;
}

// ---------------------------------------------

// is_convex проверяет, выпуклый ли многоугольник.
// Вершины приходят парами координат после count: x1, y1, x2, y2, ...
// На каждом углу считается векторное произведение соседних сторон.
// Если все ненулевые произведения одного знака, повороты в одну сторону и фигура выпуклая.
Status is_convex(const double eps, int *answer, const int count, ...) {
    Point *points;
    va_list args;
    int i;
    int sign = 0;
    int turns = 0;

    if (answer == NULL || !good_eps(eps) || count < 3) {
        return BAD_INPUT;
    }
    points = (Point *)malloc((size_t)count * sizeof(Point));
    if (points == NULL) {
        return NO_MEMORY;
    }
    va_start(args, count);
    for (i = 0; i < count; i++) {
        points[i].x = va_arg(args, double);
        points[i].y = va_arg(args, double);
        if (!isfinite(points[i].x) || !isfinite(points[i].y)) {
            va_end(args);
            free(points);
            return BAD_INPUT;
        }
    }
    va_end(args);

    for (i = 0; i < count; i++) {
        int prev = (i + count - 1) % count;
        int next = (i + 1) % count;
        double cross = (points[i].x - points[prev].x) * (points[next].y - points[i].y) -
                       (points[i].y - points[prev].y) * (points[next].x - points[i].x);
        int now;
        // почти нулевое произведение значит, что три точки лежат на одной прямой: поворота нет
        if (fabs(cross) <= eps) {
            continue;
        }
        now = cross > 0.0 ? 1 : -1;
        if (sign == 0) {
            sign = now;
        // знак поворота сменился — многоугольник вогнутый
        } else if (now != sign) {
            free(points);
            *answer = 0;
            return OK;
        }
        turns++;
    }
    free(points);
    *answer = turns > 0 || count >= 3;
    return OK;
}

// ---------------------------------------------

// polynomial считает многочлен в точке x схемой Горнера.
// Сначала степень, потом коэффициенты от старшего к свободному:
// value = (...((a_n) * x + a_(n-1)) * x + ...) * x + a_0
Status polynomial(double *result, const double x, const int degree, ...) {
    va_list args;
    double value = 0.0;
    int i;

    if (result == NULL || degree < 0 || !isfinite(x)) {
        return BAD_INPUT;
    }
    va_start(args, degree);
    for (i = 0; i <= degree; i++) {
        double coeff = va_arg(args, double);
        if (!isfinite(coeff)) {
            va_end(args);
            return BAD_INPUT;
        }
        // один шаг схемы Горнера
        value = value * x + coeff;
        if (!isfinite(value)) {
            va_end(args);
            return TOO_BIG;
        }
    }
    va_end(args);
    *result = value;
    return OK;
}

// parse_u переводит строку из системы с основанием base в целое без знака
static Status parse_u(const char *text, const int base, unsigned long long *number) {
    int i = 0;
    unsigned long long value = 0;

    if (text == NULL || number == NULL || text[0] == '\0' || base < 2 || base > 36) {
        return BAD_INPUT;
    }
    while (text[i] != '\0') {
        int digit = digit_value(text[i]);
        if (digit < 0 || digit >= base) {
            return BAD_INPUT;
        }
        if (value > (~0ULL - (unsigned)digit) / (unsigned)base) {
            return TOO_BIG;
        }
        value = value * (unsigned)base + (unsigned)digit;
        i++;
    }
    *number = value;
    return OK;
}

// is_kaprekar: число Капрекара в данной системе.
// Квадрат числа делится на две части, у правой столько же цифр, сколько у самого числа.
// Если левая и правая части в сумме дают исходное число, это число Капрекара.
// Пример в системе 10: 45^2 = 2025, 20 + 25 = 45.
static int is_kaprekar(const unsigned long long number, const int base) {
    unsigned long long square;
    unsigned long long mod = 1;
    unsigned long long copy;
    unsigned long long right;
    unsigned long long left;
    int digits = 0;
    int i;

    if (number == 0) {
        return 1;
    }
    if (number > ~0ULL / number) {
        return 0;
    }
    square = number * number;
    copy = number;
    while (copy > 0) {
        copy /= (unsigned)base;
        digits++;
    }
    for (i = 0; i < digits; i++) {
        if (mod > ~0ULL / (unsigned)base) {
            return 0;
        }
        mod *= (unsigned)base;
    }
    // mod = base^digits, поэтому остаток — правая часть, частное — левая
    right = square % mod;
    left = square / mod;
    return left + right == number;
}

// ---------------------------------------------

// kaprekar_list среди переданных строк оставляет те, что задают числа Капрекара
Status kaprekar_list(unsigned long long *found, int *found_count, const int capacity, const int base, const int count, ...) {
    va_list args;
    int i;
    int stored = 0;

    if (found == NULL || found_count == NULL || capacity < 0 || count < 0 || base < 2 || base > 36) {
        return BAD_INPUT;
    }
    va_start(args, count);
    for (i = 0; i < count; i++) {
        const char *text = va_arg(args, const char *);
        unsigned long long number = 0;
        Status code = parse_u(text, base, &number);
        if (code != OK) {
            va_end(args);
            return code;
        }
        if (is_kaprekar(number, base)) {
            if (stored >= capacity) {
                va_end(args);
                return TOO_BIG;
            }
            found[stored++] = number;
        }
    }
    va_end(args);
    *found_count = stored;
    return OK;
}

// ---------------------------------------------

// geometric_mean считает среднее геометрическое: корень степени count из произведения чисел.
// Через логарифм: exp((ln x1 + ln x2 + ... ) / count), чтобы произведение не переполнилось.
// count — последний обязательный параметр, сами числа идут за ним.
Status geometric_mean(double *result, const int count, ...) {
    va_list args;
    double sum = 0.0;
    int i;

    if (result == NULL || count <= 0) {
        return BAD_INPUT;
    }
    va_start(args, count);
    for (i = 0; i < count; i++) {
        double value = va_arg(args, double);
        if (!isfinite(value) || value <= 0.0) {
            va_end(args);
            return BAD_INPUT;
        }
        sum += log(value);
    }
    va_end(args);
    *result = exp(sum / (double)count);
    if (!isfinite(*result)) {
        return TOO_BIG;
    }
    return OK;
}

// power_rec — быстрое возведение в степень.
// Чётную степень сводим к квадрату половинной, нечётную — к base * степень на единицу меньше.
static Status power_rec(const double base, const int exponent, double *result) {
    double half;
    Status code;

    if (exponent == 0) {
        *result = 1.0;
        return OK;
    }
    if (exponent % 2 == 0) {
        code = power_rec(base, exponent / 2, &half);
        if (code != OK) {
            return code;
        }
        *result = half * half;
    } else {
        code = power_rec(base, exponent - 1, &half);
        if (code != OK) {
            return code;
        }
        *result = base * half;
    }
    if (!isfinite(*result)) {
        return TOO_BIG;
    }
    return OK;
}

// ---------------------------------------------

// fast_power возводит base в целую степень. Отрицательная степень — это 1 / base^|степень|.
Status fast_power(double *result, const double base, const int exponent) {
    if (result == NULL || !isfinite(base)) {
        return BAD_INPUT;
    }
    if (exponent < 0) {
        double up;
        Status code;
        if (base == 0.0 || exponent == INT_MIN) {
            return BAD_INPUT;
        }
        code = power_rec(base, -exponent, &up);
        if (code != OK) {
            return code;
        }
        if (up == 0.0) {
            return BAD_INPUT;
        }
        *result = 1.0 / up;
        if (!isfinite(*result)) {
            return TOO_BIG;
        }
        return OK;
    }
    return power_rec(base, exponent, result);
}

// ---------------------------------------------

// find_root ищет корень f(x) = 0 методом дихотомии на отрезке [left, right].
// На концах функция должна иметь разные знаки, иначе корня на отрезке нет.
// Отрезок каждый раз режется пополам, пока его длина не станет не больше eps.
Status find_root(double *root, const double left, const double right, const double eps, double (*function)(double)) {
    double a;
    double b;
    double fa;
    double fb;
    int i;

    if (root == NULL || function == NULL || !good_eps(eps) || !isfinite(left) || !isfinite(right) || !(left < right)) {
        return BAD_INPUT;
    }
    a = left;
    b = right;
    fa = function(a);
    fb = function(b);
    if (!isfinite(fa) || !isfinite(fb)) {
        return BAD_INPUT;
    }
    if (fabs(fa) <= eps) {
        *root = a;
        return OK;
    }
    if (fabs(fb) <= eps) {
        *root = b;
        return OK;
    }
    // одинаковый знак на концах: корень этим методом не гарантирован
    if ((fa < 0.0) == (fb < 0.0)) {
        return BAD_INPUT;
    }
    for (i = 0; i < 200 && b - a > eps; i++) {
        double mid = (a + b) / 2.0;
        double fm = function(mid);
        if (!isfinite(fm)) {
            return BAD_INPUT;
        }
        if (fabs(fm) <= eps) {
            *root = mid;
            return OK;
        }
        // корень лежит в той половине, где знак меняется
        if ((fa < 0.0) != (fm < 0.0)) {
            b = mid;
            fb = fm;
        } else {
            a = mid;
            fa = fm;
        }
    }
    *root = (a + b) / 2.0;
    if (b - a <= eps) {
        return OK;
    }
    return NOT_DONE;
}
