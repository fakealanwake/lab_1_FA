#include "array.h"

#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 20

// read_int переводит аргумент командной строки в int и отвергает лишние символы и переполнение

static Status read_int(const char *text, int *number) {
    char *end = NULL;
    long value;

    if (text == NULL || number == NULL || text[0] == '\0') {
        return BAD_INPUT;
    }
    errno = 0;
    value = strtol(text, &end, 10);
    if (end == text || *end != '\0' || errno == ERANGE || value > 2147483647L || value < -2147483647L - 1) {
        return BAD_INPUT;
    }
    *number = (int)value;
    return OK;
}

// print_array печатает массив в одну строку
static void print_array(const char *title, const int *array, const int length) {
    int i;
    printf("%s", title);
    for (i = 0; i < length; i++) {
        printf("%d%s", array[i], i + 1 == length ? "\n" : " ");
    }
}

// main делает две части задания:
// 1) массив фиксированного размера в диапазоне [a..b], затем обмен минимума и максимума
// 2) массивы A и B случайной длины, из них массив C
// read_stdin_args читает параметры одной строкой из stdin, если в командной строке их нет и ввод идёт из файла.
// Так работает запуск task9.exe < args_task09_a.txt
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

int main(int argc, char **argv) {
    int left;
    int right;
    int fixed[ARRAY_SIZE];
    int *a = NULL;
    int *b = NULL;
    int *c = NULL;
    int a_len;
    int b_len;
    Status code;

    read_stdin_args(&argc, &argv);
    if (argc != 3 || read_int(argv[1], &left) != OK || read_int(argv[2], &right) != OK || left > right) {
        fprintf(stderr, "нужно: a b, где a <= b\n");
        return 1;
    }

    srand((unsigned)time(NULL));
    code = fill_array(fixed, ARRAY_SIZE, left, right);
    if (code != OK) {
        fprintf(stderr, "не удалось заполнить массив\n");
        return 1;
    }
    print_array("до: ", fixed, ARRAY_SIZE);
    code = swap_min_max(fixed, ARRAY_SIZE);
    if (code != OK) {
        fprintf(stderr, "не удалось поменять минимум и максимум\n");
        return 1;
    }
    print_array("после: ", fixed, ARRAY_SIZE);

    // длины A и B — случайные числа от 10 до 10000
    a_len = 10 + rand() % (10000 - 10 + 1);
    b_len = 10 + rand() % (10000 - 10 + 1);
    a = (int *)malloc((size_t)a_len * sizeof(int));
    b = (int *)malloc((size_t)b_len * sizeof(int));
    c = (int *)malloc((size_t)a_len * sizeof(int));
    if (a == NULL || b == NULL || c == NULL) {
        fprintf(stderr, "нет памяти\n");
        free(a);
        free(b);
        free(c);
        return 1;
    }
    if (fill_array(a, a_len, -1000, 1000) != OK || fill_array(b, b_len, -1000, 1000) != OK ||
        nearest_sum(a, a_len, b, b_len, c) != OK) {
        fprintf(stderr, "не удалось собрать массив C\n");
        free(a);
        free(b);
        free(c);
        return 1;
    }
    printf("размер A: %d, размер B: %d\n", a_len, b_len);
    print_array("C: ", c, a_len);
    free(a);
    free(b);
    free(c);
    return 0;
}
