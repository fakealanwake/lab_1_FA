#include "number.h"

#include <limits.h>
#include <stdlib.h>

// digit_value понимает только цифры и прописные буквы: 'A' это 10, строчная 'a' здесь недопустима
static int digit_value(const char symbol) {
    if (symbol >= '0' && symbol <= '9') {
        return symbol - '0';
    }
    if (symbol >= 'A' && symbol <= 'Z') {
        return symbol - 'A' + 10;
    }
    return -1;
}

// ---------------------------------------------

// read_base читает основание системы счисления, оно должно быть от 2 до 36
Status read_base(const char *text, int *base) {
    char *end = NULL;
    long value;

    if (text == NULL || base == NULL || text[0] == '\0') {
        return BAD_INPUT;
    }
    value = strtol(text, &end, 10);
    if (end == text || *end != '\0' || value < 2 || value > 36) {
        return BAD_INPUT;
    }
    *base = (int)value;
    return OK;
}

// read_number переводит строку из системы base в целое со знаком.
// Перед числом может стоять минус. Буквы больше 9 должны быть прописными.
Status read_number(const char *text, const int base, long long *number) {
    int i = 0;
    int minus = 0;
    unsigned long long value = 0;

    if (text == NULL || number == NULL || text[0] == '\0' || base < 2 || base > 36) {
        return BAD_INPUT;
    }
    if (text[0] == '-') {
        minus = 1;
        i = 1;
        if (text[1] == '\0') {
            return BAD_INPUT;
        }
    }
    while (text[i] != '\0') {
        int digit = digit_value(text[i]);
        if (digit < 0 || digit >= base) {
            return BAD_INPUT;
        }
        // следующий шаг value * base + digit не поместится в unsigned long long
        if (value > (~0ULL - (unsigned)digit) / (unsigned)base) {
            return TOO_BIG;
        }
        value = value * (unsigned)base + (unsigned)digit;
        i++;
    }
    if (minus) {
        if (value > (unsigned long long)LLONG_MAX + 1ULL) {
            return TOO_BIG;
        }
        if (value == (unsigned long long)LLONG_MAX + 1ULL) {
            *number = LLONG_MIN;
        } else {
            *number = -(long long)value;
        }
    } else {
        if (value > (unsigned long long)LLONG_MAX) {
            return TOO_BIG;
        }
        *number = (long long)value;
    }
    return OK;
}

// ---------------------------------------------

// add_numbers складывает два long long и отдельно сообщает, если сумма не помещается
Status add_numbers(const long long left, const long long right, long long *sum) {
    if (sum == NULL) {
        return BAD_INPUT;
    }
    if (right > 0 && left > LLONG_MAX - right) {
        return TOO_BIG;
    }
    if (right < 0 && left < LLONG_MIN - right) {
        return TOO_BIG;
    }
    *sum = left + right;
    return OK;
}

// magnitude возвращает модуль числа. LLONG_MIN нельзя просто взять со знаком минус:
// противоположное к нему уже не помещается в long long.
static unsigned long long magnitude(const long long number) {
    if (number >= 0) {
        return (unsigned long long)number;
    }
    if (number == LLONG_MIN) {
        return (unsigned long long)LLONG_MAX + 1ULL;
    }
    return (unsigned long long)(-number);
}

// abs_greater истинно, если модуль left больше модуля right
int abs_greater(const long long left, const long long right) {
    unsigned long long a = magnitude(left);
    unsigned long long b = magnitude(right);
    return a > b;
}

// ---------------------------------------------

// to_base записывает число в систему с основанием base без ведущих нулей.
// Отрицательное число начинается с минуса, цифры больше 9 — прописные буквы.
Status to_base(const long long number, const int base, char *text, const int size) {
    unsigned long long value;
    char temp[80];
    int length = 0;
    int pos = 0;
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (text == NULL || size < 3 || base < 2 || base > 36) {
        return BAD_INPUT;
    }
    value = magnitude(number);
    if (value == 0) {
        temp[length++] = '0';
    }
    // остатки от деления собираются от младшей цифры к старшей, потом разворачиваются
    while (value > 0) {
        if (length >= (int)sizeof(temp)) {
            return TOO_BIG;
        }
        temp[length++] = digits[value % (unsigned)base];
        value /= (unsigned)base;
    }
    if (number < 0) {
        if (pos + 1 >= size) {
            return TOO_BIG;
        }
        text[pos++] = '-';
    }
    while (length > 0) {
        if (pos + 1 >= size) {
            return TOO_BIG;
        }
        text[pos++] = temp[--length];
    }
    text[pos] = '\0';
    return OK;
}
