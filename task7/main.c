#include "text.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// read_all читает файл целиком в память, чтобы потом разрезать его на лексемы
static Status read_all(FILE *file, char **text, size_t *length) {
    char *buffer = NULL;
    size_t size = 0;
    size_t capacity = 0;
    char chunk[1024];
    size_t got;

    if (file == NULL || text == NULL || length == NULL) {
        return BAD_INPUT;
    }
    while ((got = fread(chunk, 1, sizeof(chunk), file)) > 0) {
        if (size + got + 1 < size) {
            free(buffer);
            return NO_MEMORY;
        }
        if (size + got + 1 > capacity) {
            size_t next = capacity == 0 ? 1024 : capacity * 2;
            char *grown;
            while (next < size + got + 1) {
                next *= 2;
            }
            grown = (char *)realloc(buffer, next);
            if (grown == NULL) {
                free(buffer);
                return NO_MEMORY;
            }
            buffer = grown;
            capacity = next;
        }
        memcpy(buffer + size, chunk, got);
        size += got;
    }
    if (ferror(file)) {
        free(buffer);
        return FILE_ERROR;
    }
    if (buffer == NULL) {
        buffer = (char *)malloc(1);
        if (buffer == NULL) {
            return NO_MEMORY;
        }
    }
    buffer[size] = '\0';
    *text = buffer;
    *length = size;
    return OK;
}

// write_result записывает готовую строку в выходной файл
static int write_result(const char *path, const char *text, const size_t length) {
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "не открылся выходной файл\n");
        return 0;
    }
    if (length > 0 && fwrite(text, 1, length, file) != length) {
        fprintf(stderr, "ошибка записи\n");
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

// load_tokens открывает файл, читает его и возвращает массив лексем
static int load_tokens(const char *path, char ***tokens, int *count) {
    FILE *file;
    char *text = NULL;
    size_t length = 0;
    Status code;

    file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "не открылся файл %s\n", path);
        return 0;
    }
    code = read_all(file, &text, &length);
    fclose(file);
    if (code != OK) {
        fprintf(stderr, "ошибка чтения\n");
        free(text);
        return 0;
    }
    code = split_tokens(text, length, tokens, count);
    free(text);
    if (code != OK) {
        fprintf(stderr, "нет памяти\n");
        return 0;
    }
    return 1;
}

// read_stdin_args читает параметры одной строкой из stdin, если в командной строке их нет и ввод идёт из файла.
// Так работает запуск task7.exe < args_task07_a.txt
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

// main: флаг -r чередует лексемы двух файлов, флаг -a преобразует лексемы одного файла
int main(int argc, char **argv) {
    char flag;
    char **left = NULL;
    char **right = NULL;
    char **result = NULL;
    char *text = NULL;
    int left_count = 0;
    int right_count = 0;
    int result_count = 0;
    size_t length = 0;
    int ok;

    read_stdin_args(&argc, &argv);
    if (argc < 2 || read_flag(argv[1], &flag) != OK) {
        fprintf(stderr, "нужно: -r файл1 файл2 выход | -a вход выход\n");
        return 1;
    }

    if (flag == 'r') {
        if (argc != 5) {
            fprintf(stderr, "для -r нужны два входных файла и выходной\n");
            return 1;
        }
        if (!load_tokens(argv[2], &left, &left_count) || !load_tokens(argv[3], &right, &right_count)) {
            free_tokens(left, left_count);
            free_tokens(right, right_count);
            return 1;
        }
        if (merge_tokens(left, left_count, right, right_count, &result, &result_count) != OK) {
            fprintf(stderr, "нет памяти\n");
            free_tokens(left, left_count);
            free_tokens(right, right_count);
            return 1;
        }
        free_tokens(left, left_count);
        free_tokens(right, right_count);
        if (join_tokens(result, result_count, &text, &length) != OK) {
            fprintf(stderr, "нет памяти\n");
            free_tokens(result, result_count);
            return 1;
        }
        ok = write_result(argv[4], text, length);
        free(text);
        free_tokens(result, result_count);
        return ok ? 0 : 1;
    }

    if (argc != 4) {
        fprintf(stderr, "для -a нужны входной и выходной файлы\n");
        return 1;
    }
    if (!load_tokens(argv[2], &left, &left_count)) {
        return 1;
    }
    if (convert_tokens(left, left_count, &result, &result_count) != OK) {
        fprintf(stderr, "нет памяти\n");
        free_tokens(left, left_count);
        return 1;
    }
    free_tokens(left, left_count);
    if (join_tokens(result, result_count, &text, &length) != OK) {
        fprintf(stderr, "нет памяти\n");
        free_tokens(result, result_count);
        return 1;
    }
    ok = write_result(argv[3], text, length);
    free(text);
    free_tokens(result, result_count);
    return ok ? 0 : 1;
}
