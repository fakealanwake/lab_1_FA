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

// same_file считает пути одним файлом, даже если слэши и регистр букв различаются.
static int same_file(const char *left, const char *right) {
    char first[4096];
    char second[4096];
    size_t index;

    if (left == NULL || right == NULL) {
        return 0;
    }
    if (_fullpath(first, left, sizeof(first)) == NULL || _fullpath(second, right, sizeof(second)) == NULL) {
        return 0;
    }
    for (index = 0; first[index] != '\0' && second[index] != '\0'; index++) {
        char one = first[index];
        char two = second[index];
        if (one >= 'A' && one <= 'Z') {
            one = (char)(one - 'A' + 'a');
        }
        if (two >= 'A' && two <= 'Z') {
            two = (char)(two - 'A' + 'a');
        }
        if (one != two) {
            return 0;
        }
    }
    return first[index] == '\0' && second[index] == '\0';
}

// main для каждого числа из входного файла пишет строку: запись без нулей, основание, значение в системе 10
int main(int argc, char **argv) {
    FILE *input;
    FILE *output;
    char *token = NULL;

    read_stdin_args(&argc, &argv);
    if (argc != 3) {
        printf( "нужно: входной_файл выходной_файл\n");
        return 1;
    }
    if (same_file(argv[1], argv[2])) {
        printf("вход и выход совпадают\n");
        return 1;
    }
    input = fopen(argv[1], "rb");
    if (input == NULL) {
        printf( "не открылся входной файл\n");
        return 1;
    }
    output = fopen(argv[2], "wb");
    if (output == NULL) {
        printf( "не открылся выходной файл\n");
        fclose(input);
        return 1;
    }

    for (;;) {
        char shown[4096];
        int base = 0;
        unsigned long long value = 0;
        Status code = read_token(input, &token);
        if (code != OK) {
            printf( "ошибка чтения\n");
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
            printf( "число не помещается\n");
            fclose(input);
            fclose(output);
            return 2;
        }
        if (code != OK) {
            printf( "плохая лексема\n");
            fclose(input);
            fclose(output);
            return 1;
        }
        if (fprintf(output, "%s %d %llu\n", shown, base, value) < 0) {
            printf( "ошибка записи\n");
            fclose(input);
            fclose(output);
            return 1;
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}
