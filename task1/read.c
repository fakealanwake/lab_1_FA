#include "read.h"
#include <limits.h>
#include <stddef.h>

// ---------------------------------------------

// read_long переводит строку в целое long long.
// Допускается знак + или - в начале, дальше только цифры.
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
        
        // цифры в кодировке идут подряд, поэтому код меньше '0' или больше '9' — это не цифра
        if (text[index] < '0' || text[index] > '9') {
            return BAD_ARG;
        }

        // '7' - '0' даёт число 7
        digit = (unsigned int)(text[index] - '0');
        // следующая цифра сделала бы число больше, чем помещается в long long
        if (value > ((unsigned long long)LLONG_MAX - digit) / 10ULL) {
            //if (!(negative && value == (unsigned long long)LLONG_MAX / 10 && digit == 8 && text[index + 1] == '\0')) {
                return BAD_ARG;
            //}
        }

        value = value * 10 + digit;
        index++;
    }
    // модуль LLONG_MIN на единицу больше LLONG_MAX, его нельзя получить простым минусом
    if (negative) {
        if (value == (unsigned long long)LLONG_MAX + 1ULL) {
            *number = LLONG_MIN;
        } else {
            *number = -(long long)value;
        }
    } else {
        *number = (long long)value;
    }
    return OK;
}

// ---------------------------------------------

// read_flag принимает флаг из двух символов: -h или /h и так же для остальных букв
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
