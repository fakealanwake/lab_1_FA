#include "text.h"

#include <stdlib.h>
#include <string.h>

// is_space — разделитель лексем: пробел, табуляция или перевод строки
static int is_space(const char symbol) {
    return symbol == ' ' || symbol == '\t' || symbol == '\n' || symbol == '\r';
}

// add_token добавляет указатель на строку в динамический массив лексем
static Status add_token(char ***tokens, int *count, int *capacity, char *token) {
    if (*count >= *capacity) {
        int next = *capacity == 0 ? 16 : *capacity * 2;
        char **grown;
        if (next < *capacity) {
            return NO_MEMORY;
        }
        grown = (char **)realloc(*tokens, (size_t)next * sizeof(char *));
        if (grown == NULL) {
            return NO_MEMORY;
        }
        *tokens = grown;
        *capacity = next;
    }
    (*tokens)[*count] = token;
    (*count)++;
    return OK;
}

// ---------------------------------------------

// read_flag принимает только -r, /r, -a и /a
Status read_flag(const char *text, char *flag) {
    if (text == NULL || flag == NULL) {
        return BAD_INPUT;
    }
    if ((text[0] != '-' && text[0] != '/') || text[1] == '\0' || text[2] != '\0') {
        return BAD_INPUT;
    }
    if (text[1] != 'r' && text[1] != 'a') {
        return BAD_INPUT;
    }
    *flag = text[1];
    return OK;
}

// ---------------------------------------------

// split_tokens режет текст на лексемы. Между ними может быть сколько угодно пробелов.
Status split_tokens(const char *text, const size_t length, char ***tokens, int *count) {
    size_t i = 0;
    char **list = NULL;
    int size = 0;
    int capacity = 0;

    if (text == NULL || tokens == NULL || count == NULL) {
        return BAD_INPUT;
    }
    while (i < length) {
        size_t start;
        size_t word;
        char *copy;
        Status code;
        while (i < length && is_space(text[i])) {
            i++;
        }
        if (i >= length) {
            break;
        }
        start = i;
        while (i < length && !is_space(text[i])) {
            i++;
        }
        word = i - start;
        copy = (char *)malloc(word + 1);
        if (copy == NULL) {
            free_tokens(list, size);
            return NO_MEMORY;
        }
        memcpy(copy, text + start, word);
        copy[word] = '\0';
        code = add_token(&list, &size, &capacity, copy);
        if (code != OK) {
            free(copy);
            free_tokens(list, size);
            return code;
        }
    }
    *tokens = list;
    *count = size;
    return OK;
}

// free_tokens освобождает и сами строки, и массив указателей
void free_tokens(char **tokens, const int count) {
    int i;
    if (tokens == NULL) {
        return;
    }
    for (i = 0; i < count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

// ---------------------------------------------

// merge_tokens чередует лексемы: нечётные позиции из первого файла, чётные из второго.
// Если один файл кончился раньше, в конец дописываются остатки второго.
Status merge_tokens(char **first, const int first_count, char **second, const int second_count, char ***out, int *out_count) {
    char **list = NULL;
    int size = 0;
    int capacity = 0;
    int i1 = 0;
    int i2 = 0;
    int take_first = 1;

    if (out == NULL || out_count == NULL || first_count < 0 || second_count < 0) {
        return BAD_INPUT;
    }
    if ((first_count > 0 && first == NULL) || (second_count > 0 && second == NULL)) {
        return BAD_INPUT;
    }
    while (i1 < first_count || i2 < second_count) {
        char *copy;
        const char *src;
        Status code;
        // take_first — сейчас очередь первого файла; если его лексемы кончились, берём второй
        if (take_first && i1 < first_count) {
            src = first[i1++];
        } else if (!take_first && i2 < second_count) {
            src = second[i2++];
        } else if (i1 < first_count) {
            src = first[i1++];
        } else {
            src = second[i2++];
        }
        take_first = !take_first;
        copy = (char *)malloc(strlen(src) + 1);
        if (copy == NULL) {
            free_tokens(list, size);
            return NO_MEMORY;
        }
        memcpy(copy, src, strlen(src) + 1);
        code = add_token(&list, &size, &capacity, copy);
        if (code != OK) {
            free(copy);
            free_tokens(list, size);
            return code;
        }
    }
    *out = list;
    *out_count = size;
    return OK;
}

// write_base дописывает число в систему с основанием base, без ведущих нулей
static void write_base(char *dest, int *pos, unsigned int value, const int base) {
    char temp[16];
    int n = 0;
    const char *digits = "0123456789ABCDEF";

    if (value == 0) {
        dest[(*pos)++] = '0';
        return;
    }
    while (value > 0) {
        temp[n++] = digits[value % (unsigned)base];
        value /= (unsigned)base;
    }
    while (n > 0) {
        dest[(*pos)++] = temp[--n];
    }
}

// change_one преобразует одну лексему по её номеру (нумерация с 1):
// каждая 10-я: сначала латинские буквы в нижний регистр, потом каждый символ в ASCII в системе 4
// каждая 2-я, но не 10-я: латинские буквы в нижний регистр
// каждая 5-я, но не 10-я: каждый символ в ASCII в системе 8
// остальные лексемы не меняются
static char *change_one(const char *token, const int index) {
    size_t length = strlen(token);
    int number = index + 1;
    char *result;
    size_t i;

    // 10, 20, 30, ... проверяем раньше 2 и 5, потому что они тоже делятся на 2 и на 5
    if (number % 10 == 0) {
        int pos = 0;
        result = (char *)malloc(length * 4 + 1);
        if (result == NULL) {
            return NULL;
        }
        for (i = 0; i < length; i++) {
            unsigned char symbol = (unsigned char)token[i];
            if (symbol >= 'A' && symbol <= 'Z') {
                symbol = (unsigned char)(symbol - 'A' + 'a');
            }
            write_base(result, &pos, symbol, 4);
        }
        result[pos] = '\0';
        return result;
    }
    if (number % 2 == 0) {
        result = (char *)malloc(length + 1);
        if (result == NULL) {
            return NULL;
        }
        for (i = 0; i < length; i++) {
            char symbol = token[i];
            if (symbol >= 'A' && symbol <= 'Z') {
                symbol = (char)(symbol - 'A' + 'a');
            }
            result[i] = symbol;
        }
        result[length] = '\0';
        return result;
    }
    if (number % 5 == 0) {
        int pos = 0;
        result = (char *)malloc(length * 4 + 1);
        if (result == NULL) {
            return NULL;
        }
        for (i = 0; i < length; i++) {
            write_base(result, &pos, (unsigned char)token[i], 8);
        }
        result[pos] = '\0';
        return result;
    }
    result = (char *)malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }
    memcpy(result, token, length + 1);
    return result;
}

// ---------------------------------------------

// convert_tokens применяет change_one к каждой лексеме
Status convert_tokens(char **tokens, const int count, char ***out, int *out_count) {
    char **list = NULL;
    int size = 0;
    int capacity = 0;
    int i;

    if (out == NULL || out_count == NULL || count < 0 || (count > 0 && tokens == NULL)) {
        return BAD_INPUT;
    }
    for (i = 0; i < count; i++) {
        char *changed = change_one(tokens[i], i);
        Status code;
        if (changed == NULL) {
            free_tokens(list, size);
            return NO_MEMORY;
        }
        code = add_token(&list, &size, &capacity, changed);
        if (code != OK) {
            free(changed);
            free_tokens(list, size);
            return code;
        }
    }
    *out = list;
    *out_count = size;
    return OK;
}

// join_tokens склеивает лексемы в одну строку через один пробел
Status join_tokens(char **tokens, const int count, char **text, size_t *length) {
    size_t total = 0;
    size_t pos = 0;
    int i;
    char *buffer;

    if (text == NULL || length == NULL || count < 0 || (count > 0 && tokens == NULL)) {
        return BAD_INPUT;
    }
    for (i = 0; i < count; i++) {
        total += strlen(tokens[i]);
        if (i + 1 < count) {
            total++;
        }
    }
    buffer = (char *)malloc(total + 1);
    if (buffer == NULL) {
        return NO_MEMORY;
    }
    for (i = 0; i < count; i++) {
        size_t n = strlen(tokens[i]);
        memcpy(buffer + pos, tokens[i], n);
        pos += n;
        if (i + 1 < count) {
            buffer[pos++] = ' ';
        }
    }
    buffer[pos] = '\0';
    *text = buffer;
    *length = pos;
    return OK;
}
