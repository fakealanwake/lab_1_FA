#include "edit.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

// read_file читает весь файл в одну строку. Длину возвращает отдельно:
// в файле могут быть нулевые байты, поэтому на strlen полагаться нельзя.
static Status read_file(FILE *file, char **text, size_t *length) {
    char *buffer = NULL;
    size_t size = 0;
    size_t capacity = 0;
    char chunk[1024];
    size_t got;

    if (file == NULL || text == NULL || length == NULL) {
        return BAD_INPUT;
    }
    while ((got = fread(chunk, 1, sizeof(chunk), file)) > 0) {
        size_t i;
        if (size + got + 1 < size) {
            free(buffer);
            return NO_MEMORY;
        }
        if (size + got + 1 > capacity) {
            size_t next = capacity == 0 ? 1024 : capacity * 2;
            char *grown;
            while (next < size + got + 1) {
                if (next > (size_t)-1 / 2) {
                    free(buffer);
                    return NO_MEMORY;
                }
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
        for (i = 0; i < got; i++) {
            buffer[size + i] = chunk[i];
        }
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

// read_stdin_args читает параметры одной строкой из stdin, если в командной строке их нет и ввод идёт из файла.
// Так работает запуск task4.exe < args_task04_a.txt
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

// main проверяет аргументы, читает вход, отдаёт текст в change_text и пишет результат в файл.
int main(int argc, char **argv) {
    char action = 0;
    int own_name = 0;
    char *generated = NULL;
    const char *output_path;
    FILE *input = NULL;
    FILE *output = NULL;
    char *source = NULL;
    char *changed = NULL;
    size_t length = 0;
    size_t changed_length = 0;
    Status code;

    read_stdin_args(&argc, &argv);
    if (argc < 2 || read_action(argv[1], &action, &own_name) != OK || argc != (own_name ? 4 : 3)) {
        printf( "нужно: <-d|-i|-s|-a> файл или <-nd|-ni|-ns|-na> вход выход\n");
        return 1;
    }
    // с буквой n в флаге выходной путь — третий аргумент, иначе к имени входа добавляется out_
    if (own_name) {
        output_path = argv[3];
    } else {
        if (make_output_name(argv[2], &generated) != OK) {
            printf( "плохое имя файла\n");
            return 1;
        }
        output_path = generated;
    }

    input = fopen(argv[2], "rb");
    if (input == NULL) {
        printf( "не открылся входной файл\n");
        free(generated);
        return 1;
    }
    code = read_file(input, &source, &length);
    fclose(input);
    if (code != OK) {
        printf( "ошибка чтения\n");
        free(generated);
        free(source);
        return 1;
    }

    code = change_text(source, length, action, &changed, &changed_length);
    free(source);
    if (code != OK) {
        printf( "не хватает памяти\n");
        free(generated);
        return 1;
    }

    output = fopen(output_path, "wb");
    if (output == NULL) {
        printf( "не открылся выходной файл\n");
        free(changed);
        free(generated);
        return 1;
    }
    if (changed_length > 0 && fwrite(changed, 1, changed_length, output) != changed_length) {
        printf( "ошибка записи\n");
        fclose(output);
        free(changed);
        free(generated);
        return 1;
    }
    fclose(output);
    free(changed);
    free(generated);
    return 0;
}
