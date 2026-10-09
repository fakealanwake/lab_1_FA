#include "action.h"
#include "read.h"
#include <io.h>
#include <stdio.h>

static void print_usage(void) {
    fprintf(stderr, "нужно: число и флаг -h -p -s -e -a -f\n");
}

// read_stdin_args читает параметры одной строкой из stdin, если в командной строке их нет и ввод идёт из файла.
// Так работает запуск task1.exe < args_task01_a.txt
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

// main читает число и флаг, считает результат отдельной функцией и только потом печатает его
int main(int argc, char **argv) {
    long long number = 0;
    char flag = 0;
    Status code = OK;

    read_stdin_args(&argc, &argv);
    if (argc != 3 || read_long(argv[1], &number) != OK || read_flag(argv[2], &flag) != OK) {
        print_usage();
        return 1;
    }

    switch (flag) {
        
        /** Вывести в консоль натуральные числа в пределах 100 включительно, кратные x. 
        Если таковых нету – вывести соответствующее сообщение;
         - аргумент 1 - число
         - флаг -h
        **/
        case 'h': {
            long long list[100];
            int count = 0;
            int index;
            code = collect_multiples(number, list, &count);
            if (code == OK) {
                if (count == 0) {
                    printf("Кратных нет\n");
                } else {
                    for (index = 0; index < count; index++) {
                        printf("%lld%s", list[index], index + 1 == count ? "\n" : " ");
                    }
                }
            }
            break;
        }
        
        /** определить, является ли число x простым; является ли x составным
        - аргумент 1 - число
        - флаг -p
       **/
      case 'p': {
            int kind = 0;
            code = classify_number(number, &kind);
            if (code == OK) {
                if (kind == 1) {
                    printf("Простое\n");
                } else if (kind == 2) {
                    printf("Составное\n");
                } else {
                    printf("Ни простое, ни составное\n");
                }
            }
            break;
        }

        /** разделить число x на отдельные цифры системы счисления с основанием 16
         и вывести отдельно каждую цифру числа, разделяя их пробелом, 
         от старших разрядов к младшим, без ведущих нулей в строковом представлении;
        - аргумент 1 - число
        - флаг -s
        **/
        case 's': {
            char text[40];
            int index;
            code = format_hex(number, text, (int)sizeof(text));
            if (code == OK) {
                for (index = 0; text[index] != '\0'; index++) {
                    if (index > 0) {
                        printf(" ");
                    }
                    printf("%c", text[index]);
                }
                printf("\n");
            }
            break;
        }

        /** вывести таблицу степеней (для всех показателей в диапазоне от 1 до x) оснований от 1 до 10; 
        для этого флага работает ограничение на вводимое число: x должен быть не больше 10;
        - аргумент 1 - число
        - флаг -e
        **/
        case 'e': {
            unsigned long long table[11][11];
            int base;
            int power;
            code = fill_powers(number, table);
            if (code == OK) {
                for (base = 1; base <= 10; base++) {
                    printf("%d:", base);
                    for (power = 1; power <= (int)number; power++) {
                        printf(" %llu", table[base][power]);
                    }
                    printf("\n");
                }
            }
            break;
        }

        /** ычислить сумму всех натуральных чисел от 1 до x и вывести полученное значение в консоль; 
        - аргумент 1 - число
        - флаг -a
       **/
        case 'a': {
            unsigned long long total = 0;
            code = sum_naturals(number, &total);
            if (code == OK) {
                printf("%llu\n", total);
            }
            break;
        }
        
        /** вычислить факториал x и вывести полученное значение в консоль.
        - аргумент 1 - число
        - флаг -f
        **/
         case 'f': {
            unsigned long long result = 0;
            code = factorial(number, &result);
            if (code == OK) {
                printf("%llu\n", result);
            }
            break;
    }
    default:
        fprintf(stderr, "неизвестный флаг\n");
        return 1;
    }

    // общий разбор статуса: расчёт не влез в тип или число не подходит этому флагу
    if (code == TOO_BIG) {
        fprintf(stderr, "число не помещается\n");
        return 2;
    }

    if (code != OK) {
        fprintf(stderr, "для этого флага число не подходит\n");
        return 1;
    }
    return 0;
}
