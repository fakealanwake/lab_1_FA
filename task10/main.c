#include "number.h"

#include <stdio.h>
#include <string.h>

// read_line читает одну строку. Слишком длинная строка без перевода строки считается ошибкой.
static int read_line(char *buffer, const int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0;
    }
    if (strchr(buffer, '\n') == NULL && !feof(stdin)) {
        return 0;
    }
    return 1;
}

// trim_end убирает перевод строки в конце введённой строки
static void trim_end(char *text) {
    int n = (int)strlen(text);
    while (n > 0 && (text[n - 1] == '\n' || text[n - 1] == '\r')) {
        text[--n] = '\0';
    }
}

// print_number печатает число в системе 10 и в системах 9, 18, 27 и 36
static void print_number(const char *title, const long long number) {
    char text[80];
    int bases[4] = {9, 18, 27, 36};
    int i;

    printf("%s: ", title);
    if (to_base(number, 10, text, (int)sizeof(text)) != OK) {
        printf("ошибка\n");
        return;
    }
    printf("%s\n", text);
    for (i = 0; i < 4; i++) {
        if (to_base(number, bases[i], text, (int)sizeof(text)) != OK) {
            printf("основание %d: ошибка\n", bases[i]);
        } else {
            printf("основание %d: %s\n", bases[i], text);
        }
    }
}

// main читает основание, потом числа до строки Stop.
// По ходу хранит сумму и число с наибольшим модулем.
int main(void) {
    char line[4096];
    int base = 0;
    long long max_value = 0;
    long long sum = 0;
    int has_number = 0;

    printf("основание от 2 до 36: ");
    if (!read_line(line, (int)sizeof(line))) {
        printf( "не удалось прочитать основание\n");
        return 1;
    }
    trim_end(line);
    if (read_base(line, &base) != OK) {
        printf( "основание должно быть от 2 до 36\n");
        return 1;
    }

    printf("числа в этом основании, конец — Stop:\n");
    for (;;) {
        long long number = 0;
        Status code;
        if (!read_line(line, (int)sizeof(line))) {
            printf( "ввод кончился без Stop\n");
            return 1;
        }
        trim_end(line);
        // ввод заканчивается ровно строкой Stop
        if (strcmp(line, "Stop") == 0) {
            break;
        }
        code = read_number(line, base, &number);
        if (code == TOO_BIG) {
            printf( "число не помещается\n");
            return 2;
        }
        if (code != OK) {
            printf( "плохое число: буквы должны быть прописными\n");
            return 1;
        }
        if (!has_number) {
            max_value = number;
            sum = number;
            has_number = 1;
        } else {
            long long next = 0;
            // обновляем максимум по модулю, при равенстве оставляем уже найденное число
            if (abs_greater(number, max_value)) {
                max_value = number;
            }
            if (add_numbers(sum, number, &next) != OK) {
                printf( "сумма не помещается\n");
                return 2;
            }
            sum = next;
        }
    }

    if (!has_number) {
        printf( "чисел не было\n");
        return 1;
    }
    print_number("максимальное по модулю", max_value);
    print_number("сумма", sum);
    return 0;
}
