#include "edit.h"

#include <stdlib.h>
#include <string.h>

// ---------------------------------------------

// is_digit проверяет, является ли символ арабской цифрой
static int is_digit(const unsigned char symbol) {
    return symbol >= '0' && symbol <= '9';
}

// is_latin проверяет, является ли символ буквой латинского алфавита
static int is_latin(const unsigned char symbol) {
    return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z');
}

// push_char добавляет один символ в конец динамического буфера.
// Если места не хватает, буфер увеличивается вдвое.
static Status push_char(char **buffer, size_t *length, size_t *capacity, const char symbol) {
    if (*length + 2 > *capacity) {
        size_t next = *capacity == 0 ? 64 : *capacity * 2;
        char *grown;
        if (next < *capacity) {
            return NO_MEMORY;
        }
        grown = (char *)realloc(*buffer, next);
        if (grown == NULL) {
            return NO_MEMORY;
        }
        *buffer = grown;
        *capacity = next;
    }
    (*buffer)[(*length)++] = symbol;
    return OK;
}

// push_hex записывает ASCII-код символа в шестнадцатеричном виде, без ведущих нулей.
// Цифры собираются с конца, потом выводятся от старшей к младшей.
static Status push_hex(char **buffer, size_t *length, size_t *capacity, unsigned char symbol) {
    char temp[8];
    int count = 0;
    int i;
    const char *digits = "0123456789ABCDEF";

    if (symbol == 0) {
        return push_char(buffer, length, capacity, '0');
    }
    while (symbol > 0) {
        temp[count++] = digits[symbol % 16];
        symbol /= 16;
    }
    for (i = count - 1; i >= 0; i--) {
        Status code = push_char(buffer, length, capacity, temp[i]);
        if (code != OK) {
            return code;
        }
    }
    return OK;
}

// push_int записывает неотрицательное целое десятичными цифрами
static Status push_int(char **buffer, size_t *length, size_t *capacity, int value) {
    char temp[16];
    int count = 0;

    if (value == 0) {
        return push_char(buffer, length, capacity, '0');
    }
    while (value > 0) {
        temp[count++] = (char)('0' + value % 10);
        value /= 10;
    }
    while (count > 0) {
        Status code = push_char(buffer, length, capacity, temp[--count]);
        if (code != OK) {
            return code;
        }
    }
    return OK;
}

// ---------------------------------------------

// read_action разбирает флаг.
// -d /d — действие d, имя выхода программа придумает сама.
// -nd /nd — действие d, имя выхода передано отдельно. То же для i, s, a.
Status read_action(const char *text, char *action, int *own_name) {
    char letter;
    int pos;

    if (text == NULL || action == NULL || own_name == NULL) {
        return BAD_INPUT;
    }
    if (text[0] != '-' && text[0] != '/') {
        return BAD_INPUT;
    }
    // второй символ n значит, что путь к выходному файлу задан отдельно
    if (text[1] == 'n') {
        *own_name = 1;
        pos = 2;
    } else {
        *own_name = 0;
        pos = 1;
    }
    letter = text[pos];
    if (letter == '\0' || text[pos + 1] != '\0') {
        return BAD_INPUT;
    }
    if (letter != 'd' && letter != 'i' && letter != 's' && letter != 'a') {
        return BAD_INPUT;
    }
    *action = letter;
    return OK;
}

// ---------------------------------------------

// make_output_name приписывает префикс out_ к имени файла, не трогая папку.
// dir/a.txt превращается в dir/out_a.txt
Status make_output_name(const char *input, char **output) {
    const char *name = input;
    const char *p;
    size_t dir_len;
    size_t name_len;
    char *result;

    if (input == NULL || output == NULL || input[0] == '\0') {
        return BAD_INPUT;
    }
    // name указывает на имя после последнего слэша
    for (p = input; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\') {
            name = p + 1;
        }
    }
    if (name[0] == '\0') {
        return BAD_INPUT;
    }
    dir_len = (size_t)(name - input);
    name_len = strlen(name);
    result = (char *)malloc(dir_len + 4 + name_len + 1);
    if (result == NULL) {
        return NO_MEMORY;
    }
    memcpy(result, input, dir_len);
    memcpy(result + dir_len, "out_", 4);
    memcpy(result + dir_len + 4, name, name_len + 1);
    *output = result;
    return OK;
}

// ---------------------------------------------

// change_text меняет текст по флагу и возвращает новый буфер.
// d — выкинуть арабские цифры
// i — для каждой строки записать, сколько в ней латинских букв
// s — для каждой строки записать, сколько в ней символов кроме букв, цифр и пробела
// a — каждый символ, который не цифра, заменить его ASCII-кодом в системе 16
Status change_text(const char *text, const size_t length, const char action, char **result, size_t *result_length) {
    char *buffer = NULL;
    size_t size = 0;
    size_t capacity = 0;
    size_t i;
    int count = 0;
    int line_open = 0;
    Status code = OK;

    if (text == NULL || result == NULL || result_length == NULL) {
        return BAD_INPUT;
    }
    if (action != 'd' && action != 'i' && action != 's' && action != 'a') {
        return BAD_INPUT;
    }

    for (i = 0; i < length && code == OK; i++) {
        unsigned char symbol = (unsigned char)text[i];
        // d: цифры пропускаем, остальные символы копируем как есть
        if (action == 'd') {
            if (!is_digit(symbol)) {
                code = push_char(&buffer, &size, &capacity, (char)symbol);
            }
        } else if (action == 'a') {
            if (is_digit(symbol)) {
                code = push_char(&buffer, &size, &capacity, (char)symbol);
            } else {
                code = push_hex(&buffer, &size, &capacity, symbol);
            }
        // i и s считаются построчно: на перевод строки записываем накопленное число
        } else if (symbol == '\n') {
            code = push_int(&buffer, &size, &capacity, count);
            if (code == OK) {
                code = push_char(&buffer, &size, &capacity, '\n');
            }
            count = 0;
            line_open = 0;
        } else if (symbol != '\r') {
            line_open = 1;
            if (action == 'i' && is_latin(symbol)) {
                count++;
            }
            // s считает символы, которые не буква, не цифра и не пробел
            if (action == 's' && !is_latin(symbol) && !is_digit(symbol) && symbol != ' ') {
                count++;
            }
        }
    }

    // если файл кончился без перевода строки, последнюю строку всё равно надо записать
    if (code == OK && line_open && (action == 'i' || action == 's')) {
        code = push_int(&buffer, &size, &capacity, count);
        if (code == OK) {
            code = push_char(&buffer, &size, &capacity, '\n');
        }
    }

    if (code != OK) {
        free(buffer);
        return code;
    }
    if (buffer == NULL) {
        buffer = (char *)malloc(1);
        if (buffer == NULL) {
            return NO_MEMORY;
        }
        size = 0;
    }
    buffer[size] = '\0';
    *result = buffer;
    *result_length = size;
    return OK;
}
