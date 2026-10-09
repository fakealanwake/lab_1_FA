#include "convert.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

// is_space — разделитель чисел во входном файле
static int is_space(const int symbol) {
    return symbol == ' ' || symbol == '\t' || symbol == '\n' || symbol == '\r';
}

// read_token читает очередное слово, пропуская пробелы перед ним.
// Если слов больше нет, возвращает OK и пустой указатель.
static Status read_token(FILE *file, char **token) {
    int ch;
    char *buffer = NULL;
    size_t length = 0;
    size_t capacity = 0;

    if (file == NULL || token == NULL) {
        return BAD_INPUT;
    }
    *token = NULL;
    ch = fgetc(file);
    while (is_space(ch)) {
        ch = fgetc(file);
    }
    if (ch == EOF) {
        if (ferror(file)) {
            return BAD_INPUT;
        }
        return OK;
    }
    while (ch != EOF && !is_space(ch)) {
        if (length + 2 > capacity) {
            size_t next = capacity == 0 ? 32 : capacity * 2;
            char *grown = (char *)realloc(buffer, next);
            if (grown == NULL) {
                free(buffer);
                return BAD_INPUT;
            }
            buffer = grown;
            capacity = next;
        }
        buffer[length++] = (char)ch;
        ch = fgetc(file);
    }
    if (ch == EOF && ferror(file)) {
        free(buffer);
        return BAD_INPUT;
    }
    buffer[length] = '\0';
    *token = buffer;
    return OK;
}

// read_stdin_args читает параметры одной строкой из stdin, если в командной строке их нет и ввод идёт из файла.
// Так работает запуск task8.exe < args_task08_a.txt
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

// main для каждого числа из входного файла пишет строку: запись без нулей, основание, значение в системе 10
int main(int argc, char **argv) {
    FILE *input;
    FILE *output;
    char *token = NULL;

    read_stdin_args(&argc, &argv);
    if (argc != 3) {
        fprintf(stderr, "нужно: входной_файл выходной_файл\n");
        return 1;
    }
    input = fopen(argv[1], "rb");
    if (input == NULL) {
        fprintf(stderr, "не открылся входной файл\n");
        return 1;
    }
    output = fopen(argv[2], "wb");
    if (output == NULL) {
        fprintf(stderr, "не открылся выходной файл\n");
        fclose(input);
        return 1;
    }

    for (;;) {
        char shown[4096];
        int base = 0;
        unsigned long long value = 0;
        Status code = read_token(input, &token);
        if (code != OK) {
            fprintf(stderr, "ошибка чтения\n");
            free(token);
            fclose(input);
            fclose(output);
            return 1;
        }
        if (token == NULL) {
            break;
        }
        code = convert_number(token, shown, (int)sizeof(shown), &base, &value);
        free(token);
        token = NULL;
        if (code == TOO_BIG) {
            fprintf(stderr, "число не помещается\n");
            fclose(input);
            fclose(output);
            return 2;
        }
        if (code != OK) {
            fprintf(stderr, "плохая лексема\n");
            fclose(input);
            fclose(output);
            return 1;
        }
        if (fprintf(output, "%s %d %llu\n", shown, base, value) < 0) {
            fprintf(stderr, "ошибка записи\n");
            fclose(input);
            fclose(output);
            return 1;
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}
