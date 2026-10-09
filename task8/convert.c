#include "convert.h"

#include <stddef.h>

// digit_value переводит символ в значение цифры. Буквы a и A означают одно и то же.
static int digit_value(const char symbol) {
    if (symbol >= '0' && symbol <= '9') {
        return symbol - '0';
    }
    if (symbol >= 'A' && symbol <= 'Z') {
        return symbol - 'A' + 10;
    }
    if (symbol >= 'a' && symbol <= 'z') {
        return symbol - 'a' + 10;
    }
    return -1;
}

// to_upper переводит латинскую букву в прописную, остальные символы не трогает
static char to_upper(const char symbol) {
    if (symbol >= 'a' && symbol <= 'z') {
        return (char)(symbol - 'a' + 'A');
    }
    return symbol;
}

// ---------------------------------------------

// convert_number для строки, у которой забыто основание, находит минимальное основание из [2..36].
// Оно на единицу больше самой старшей цифры в записи.
// Например, в "10A" старшая цифра A = 10, значит минимальное основание 11.
// shown — та же запись без ведущих нулей, value — число в десятичной системе.
Status convert_number(const char *text, char *shown, const int shown_size, int *base, unsigned long long *value) {
    int i;
    int max_digit = 0;
    int start;
    int pos;
    unsigned long long number = 0;
    int system;

    if (text == NULL || shown == NULL || base == NULL || value == NULL || text[0] == '\0' || shown_size < 2) {
        return BAD_INPUT;
    }
    for (i = 0; text[i] != '\0'; i++) {
        int digit = digit_value(text[i]);
        if (digit < 0) {
            return BAD_INPUT;
        }
        if (digit > max_digit) {
            max_digit = digit;
        }
    }
    // цифра 0 дала бы основание 1, но минимальное допустимое основание — 2
    system = max_digit + 1;
    if (system < 2) {
        system = 2;
    }
    start = 0;
    // ведущие нули убираем, но один ноль у числа 0 оставляем
    while (text[start] == '0' && text[start + 1] != '\0') {
        start++;
    }
    pos = 0;
    for (i = start; text[i] != '\0'; i++) {
        if (pos + 1 >= shown_size) {
            return TOO_BIG;
        }
        shown[pos++] = to_upper(text[i]);
    }
    shown[pos] = '\0';

    // перевод в десятичную систему: на каждом шаге number = number * основание + цифра
    for (i = 0; text[i] != '\0'; i++) {
        int digit = digit_value(text[i]);
        if (number > (~0ULL - (unsigned)digit) / (unsigned)system) {
            return TOO_BIG;
        }
        number = number * (unsigned)system + (unsigned)digit;
    }
    *base = system;
    *value = number;
    return OK;
}
