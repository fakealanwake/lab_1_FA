#include "action.h"
#include <limits.h>
#include <stddef.h>

// ---------------------------------------------

// collect_multiples собирает натуральные числа от 1 до 100, которые делятся на number.
// Если таких нет, count остаётся нулём, и main пишет «Кратных нет».
Status collect_multiples(const long long number, long long *list, int *count) {
    long long step;
    long long value;

    if (list == NULL || count == NULL) {
        return BAD_ARG;
    }
    *count = 0;
    // на ноль делить нельзя, кратных нулю не ищем
    if (number == 0) {
        return BAD_ARG;
    }
    if (number == LLONG_MIN) {
        return OK;
    }
    // кратные отрицательному числу те же, что и кратные его модулю
    step = number < 0 ? -number : number;
    if (step > 100) {
        return OK;
    }
    for (value = step; value <= 100; value += step) {
        list[*count] = value;
        (*count)++;
    }
    return OK;
}

// ---------------------------------------------

// classify_number определяет вид числа:
// 1 — простое, 2 — составное, 0 — ни то ни другое (так бывает у чисел меньше 2)
Status classify_number(const long long number, int *kind) {
    long long divisor;

    if (kind == NULL) {
        return BAD_ARG;
    }
    if (number <= 1) {
        *kind = 0;
        return OK;
    }
    if (number % 2 == 0) {
        *kind = number == 2 ? 1 : 2;
        return OK;
    }
    // делители достаточно проверять до корня из number: дальше пары делителей уже встречались
    for (divisor = 3; divisor <= number / divisor; divisor += 2) {
        if (number % divisor == 0) {
            *kind = 2;
            return OK;
        }
    }
    *kind = 1;
    return OK;
}

// ---------------------------------------------

// format_hex записывает число в системе с основанием 16 без ведущих нулей.
// Цифры сначала получаются с младшей, потом переносятся в text от старшей к младшей.
Status format_hex(const long long number, char *text, const int capacity) {
    unsigned long long value;
    char digits[32];
    int length = 0;
    int pos = 0;
    const char *alphabet = "0123456789ABCDEF";

    if (text == NULL || capacity < 2) {
        return BAD_ARG;
    }
    if (number == LLONG_MIN) {
        value = (unsigned long long)LLONG_MAX + 1ULL;
    } else if (number < 0) {
        value = (unsigned long long)(-number);
    } else {
        value = (unsigned long long)number;
    }
    if (value == 0) {
        digits[length++] = '0';
    }
    while (value > 0) {
        // остаток от деления на 16 — это очередная шестнадцатеричная цифра
        digits[length++] = alphabet[value % 16ULL];
        value /= 16ULL;
    }
    if (number < 0) {
        //if (pos + 1 >= capacity) {
        //    return TOO_BIG;
        //}
        text[pos++] = '-';
    }
    while (length > 0) {
        if (pos + 1 >= capacity) {
            return TOO_BIG;
        }
        // digits лежит задом наперёд, поэтому забираем цифры с конца
        text[pos++] = digits[--length];
    }
    text[pos] = '\0';
    return OK;
}

// ---------------------------------------------

// fill_powers заполняет таблицу степеней: строка — основание от 1 до 10,
// столбец — показатель от 1 до degree. degree должен быть от 1 до 10.
Status fill_powers(const long long degree, unsigned long long table[11][11]) {
    int base;
    int power;

    if (table == NULL || degree < 1 || degree > 10) {
        return BAD_ARG;
    }
    for (base = 1; base <= 10; base++) {
        unsigned long long current = 1;
        for (power = 1; power <= (int)degree; power++) {
            // base^power = base^(power-1) * base, поэтому степень копится умножением
            current *= (unsigned long long)base;
            table[base][power] = current;
        }
    }
    return OK;
}

// ---------------------------------------------

// sum_naturals считает 1 + 2 + ... + number по формуле n * (n + 1) / 2.
// Деление на 2 делается до умножения и только у чётного множителя, чтобы сумма не переполнилась раньше времени.
Status sum_naturals(const long long number, unsigned long long *total) {
    unsigned long long value;

    if (total == NULL || number < 1) {
        return BAD_ARG;
    }
    value = (unsigned long long)number;
    // если n чётное, делим n; если нечётное, делится n + 1. Остатка не остаётся.
    if (value % 2ULL == 0ULL) {
        if (value / 2ULL > ~0ULL / (value + 1ULL)) {
            return TOO_BIG;
        }
        *total = (value / 2ULL) * (value + 1ULL);
    } else {
        if (value > ~0ULL / ((value + 1ULL) / 2ULL)) {
            return TOO_BIG;
        }
        *total = value * ((value + 1ULL) / 2ULL);
    }
    return OK;
}

// ---------------------------------------------

// factorial считает 1 * 2 * ... * number. 0! равен 1.
// В unsigned long long помещается факториал только до 20 включительно.
Status factorial(const long long number, unsigned long long *result) {
    long long factor;
    unsigned long long current = 1;

    if (result == NULL || number < 0) {
        return BAD_ARG;
    }
    if (number > 20) {
        return TOO_BIG;
    }
    for (factor = 2; factor <= number; factor++) {
        current *= (unsigned long long)factor;
    }
    *result = current;
    return OK;
}
