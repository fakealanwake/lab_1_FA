#include "calc.h"

#include <math.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

// read_double переводит строку в double.
// strtod пишет в end адрес первого символа, который не вошёл в число.
static Status read_double(const char *text, double *number) {
    char *end = NULL;
    double value;

    if (text == NULL || number == NULL || text[0] == '\0') {
        return BAD_INPUT;
    }
    value = strtod(text, &end);
    // end == text — числа в строке нет; *end != '\0' — после числа остался мусор
    if (end == text || *end != '\0' || !isfinite(value)) {
        return BAD_INPUT;
    }
    *number = value;
    return OK;
}

static void show(const char *name, const Status code, const double value) {
    if (code == OK) {
        printf("%s %.10f\n", name, value);
    } else if (code == NOT_DONE) {
        printf("%s не сошёлся\n", name);
    } else {
        printf("%s неверные данные\n", name);
    }
}

// read_stdin_args читает параметры одной строкой из stdin, если в командной строке их нет и ввод идёт из файла.
// Так работает запуск task5.exe < args_task05_a.txt
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

// main считает четыре ряда при заданных eps и x, затем четыре интеграла. x нужен только рядам.
int main(int argc, char **argv) {
    double eps;
    double x;
    double value;
    Status code;

    read_stdin_args(&argc, &argv);
    if (argc != 3 || read_double(argv[1], &eps) != OK || !(eps > 0.0) || read_double(argv[2], &x) != OK) {
        fprintf(stderr, "нужно: эпсилон и x\n");
        return 1;
    }

    printf("ряды\n");
    code = row_a(x, eps, &value);
    show("  a:", code, value);
    code = row_b(x, eps, &value);
    show("  b:", code, value);
    code = row_c(x, eps, &value);
    show("  c:", code, value);
    code = row_d(x, eps, &value);
    show("  d:", code, value);

    printf("интегралы\n");
    code = integral_a(eps, &value);
    show("  a:", code, value);
    code = integral_b(eps, &value);
    show("  b:", code, value);
    code = integral_c(eps, &value);
    show("  c:", code, value);
    code = integral_d(eps, &value);
    show("  d:", code, value);
    return 0;
}
