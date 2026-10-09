#include "func.h"

#include <math.h>
#include <stdio.h>

// три уравнения для демонстрации дихотомии: x^2 = 2, e^x = 2, cos x = 0
static double f_sqrt2(double x) {
    return x * x - 2.0;
}

static double f_ln2(double x) {
    return exp(x) - 2.0;
}

static double f_half_pi(double x) {
    return cos(x);
}

static void show_status(const char *name, const Status code) {
    if (code == OK) {
        return;
    }
    if (code == NO_MEMORY) {
        printf("%s: нет памяти\n", name);
    } else if (code == TOO_BIG) {
        printf("%s: число не помещается\n", name);
    } else if (code == NOT_DONE) {
        printf("%s: не сошлось\n", name);
    } else {
        printf("%s: плохие данные\n", name);
    }
}

// main по очереди показывает все шесть функций задачи на фиксированных примерах
int main(void) {
    const double eps = 1e-9;
    int answer = 0;
    double value = 0.0;
    unsigned long long found[8];
    int found_count = 0;
    int i;
    Status code;

    code = is_convex(eps, &answer, 4, 0.0, 0.0, 2.0, 0.0, 2.0, 2.0, 0.0, 2.0);
    show_status("выпуклый квадрат", code);
    if (code == OK) {
        printf("квадрат: %s\n", answer ? "выпуклый" : "не выпуклый");
    }

    code = is_convex(eps, &answer, 5, 0.0, 0.0, 4.0, 0.0, 1.0, 1.0, 4.0, 3.0, 0.0, 3.0);
    show_status("вогнутый", code);
    if (code == OK) {
        printf("пятиугольник: %s\n", answer ? "выпуклый" : "не выпуклый");
    }

    code = polynomial(&value, 2.0, 2, 2.0, 3.0, 4.0);
    show_status("многочлен", code);
    if (code == OK) {
        printf("2x^2 + 3x + 4 при x = 2: %.6g\n", value);
    }

    code = kaprekar_list(found, &found_count, 8, 10, 5, "0", "1", "45", "46", "297");
    show_status("капрекар", code);
    if (code == OK) {
        printf("числа Капрекара в основании 10:");
        for (i = 0; i < found_count; i++) {
            printf(" %llu", found[i]);
        }
        printf("\n");
    }

    code = geometric_mean(&value, 2, 4.0, 9.0);
    show_status("среднее геометрическое", code);
    if (code == OK) {
        printf("среднее геометрическое 4 и 9: %.6g\n", value);
    }

    code = fast_power(&value, 2.0, 10);
    show_status("степень", code);
    if (code == OK) {
        printf("2^10 = %.6g\n", value);
    }
    code = fast_power(&value, 2.0, -3);
    show_status("отрицательная степень", code);
    if (code == OK) {
        printf("2^(-3) = %.6g\n", value);
    }

    code = find_root(&value, 1.0, 2.0, eps, f_sqrt2);
    show_status("корень x^2 - 2", code);
    if (code == OK) {
        printf("корень x^2 = 2 на [1, 2]: %.10f\n", value);
    }
    code = find_root(&value, 0.0, 1.0, eps, f_ln2);
    show_status("корень e^x - 2", code);
    if (code == OK) {
        printf("корень e^x = 2 на [0, 1]: %.10f\n", value);
    }
    code = find_root(&value, 0.0, 2.0, eps, f_half_pi);
    show_status("корень cos", code);
    if (code == OK) {
        printf("корень cos x = 0 на [0, 2]: %.10f\n", value);
    }
    return 0;
}
