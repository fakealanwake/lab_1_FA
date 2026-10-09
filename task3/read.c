#include "read.h"
#include <limits.h>
#include <math.h>
#include <stdlib.h>

Status read_long(const char *text, long long *number) {
    size_t index = 0;
    int negative = 0;
    unsigned long long value = 0;

    if (text == NULL || number == NULL || text[0] == '\0') {
        return BAD_ARG;
    }
    if (text[0] == '+' || text[0] == '-') {
        negative = text[0] == '-';
        index = 1;
        if (text[1] == '\0') {
            return BAD_ARG;
        }
    }
    while (text[index] != '\0') {
        unsigned int digit;
        if (text[index] < '0' || text[index] > '9') {
            return BAD_ARG;
        }
        digit = (unsigned int)(text[index] - '0');
        if (value > ((unsigned long long)LLONG_MAX - digit) / 10ULL) {
            if (!(negative && value == (unsigned long long)LLONG_MAX / 10ULL && digit == 8 && text[index + 1] == '\0')) {
                return BAD_ARG;
            }
        }
        value = value * 10ULL + digit;
        index++;
    }
    if (negative && value == (unsigned long long)LLONG_MAX + 1ULL) {
        *number = LLONG_MIN;
    } else if (negative) {
        *number = -(long long)value;
    } else {
        *number = (long long)value;
    }
    return OK;
}

Status read_double(const char *text, double *number) {
    char *end = NULL;
    double value;

    if (text == NULL || number == NULL || text[0] == '\0') {
        return BAD_ARG;
    }
    /** strtod читает из строки число с плавающей точкой и возвращает его как double. 
    Второй аргумент — адрес указателя, куда функция записывает место, на котором разбор остановился. */
    value = strtod(text, &end);
    
    //** isfinite из math.h проверяет, что double — обычное конечное число. 
    // Оно возвращает истину для нуля, обычных и очень маленьких значений и ложь для */
    if (end == text || *end != '\0' || !isfinite(value)) {
        return BAD_ARG;
    }
    *number = value;
    return OK;
}

Status read_flag(const char *text, char *flag) {
    if (text == NULL || flag == NULL) {
        return BAD_ARG;
    }
    if ((text[0] != '-' && text[0] != '/') || text[1] == '\0' || text[2] != '\0') {
        return BAD_ARG;
    }
    *flag = text[1];
    return OK;
}
