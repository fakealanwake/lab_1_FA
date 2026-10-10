#include "calc.h"
#include "read.h"
#include <math.h>
#include <io.h>
#include <stdio.h>

// show печатает имя способа и найденное значение.
// Если за отведённые шаги точность не достигнута, рядом пишется «не сошлось».
static void show(const char *name, const Status code, const double value) {
    if (code == OK) {
        printf("%-18s %.10f\n", name, value);
    } else if (code == NOT_DONE) {
        printf("%-18s %.10f  (не сошлось)\n", name, value);
    } else {
        printf("%-18s ошибка\n", name);
    }
}

// read_stdin_args читает параметры одной строкой из stdin, если в командной строке их нет и ввод идёт из файла.
// Так работает запуск task2.exe < args_task02_a.txt
static void read_stdin_args(int *argc, char ***argv) {
    static char line[4096];
    static char *stored[32];
    char *p;
    int count = 1;

    if (*argc != 1 || _isatty(_fileno(stdin)) || fgets(line, (int)sizeof(line), stdin) == NULL) {
        return;
    }
    stored[0] = (*argv)[0];
    p = line;
    while (*p != '\0' && count < 32) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        stored[count++] = p;
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') {
            p++;
        }
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
    }
    *argc = count;
    *argv = stored;
}

// main получает эпсилон и для каждой константы показывает три способа: ряд, предел и уравнение
int main(int argc, char **argv) {
    double eps = 0.0;
    double value = 0.0;
    Status code;

    read_stdin_args(&argc, &argv);
    if (argc != 2 || read_double(argv[1], &eps) != OK || !(eps > 0.0) || !isfinite(eps)) {
        printf( "нужно: положительный эпсилон\n");
        return 1;
    }

    code = exp_by_series(eps, &value);
    show("e ряд", code, value);
    code = exp_by_limit(eps, &value);
    show("e предел", code, value);
    code = exp_by_root(eps, &value);
    show("e уравнение", code, value);
    code = pi_by_series(eps, &value);
    show("pi ряд", code, value);
    code = pi_by_product(eps, &value);
    show("pi предел", code, value);
    code = pi_by_root(eps, &value);
    show("pi уравнение", code, value);
    code = ln2_by_series(eps, &value);
    show("ln2 ряд", code, value);
    code = ln2_by_limit(eps, &value);
    show("ln2 предел", code, value);
    code = ln2_by_root(eps, &value);
    show("ln2 уравнение", code, value);
    code = sqrt2_by_newton(eps, &value);
    show("sqrt2 предел", code, value);
    code = sqrt2_by_product(eps, &value);
    show("sqrt2 произведение", code, value);
    code = sqrt2_by_root(eps, &value);
    show("sqrt2 уравнение", code, value);
    code = gamma_by_series(eps, &value);
    show("gamma ряд", code, value);
    code = gamma_by_limit(eps, &value);
    show("gamma предел", code, value);
    code = gamma_by_primes(eps, &value);
    show("gamma уравнение", code, value);
    return 0;
}
