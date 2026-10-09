#include "read.h"
#include "solve.h"
#include <io.h>
#include <stdio.h>

// read_stdin_args читает параметры одной строкой из stdin, если в командной строке их нет и ввод идёт из файла.
// Так работает запуск task3.exe < args_task03_a.txt
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

int main(int argc, char **argv) {
    char flag = 0;
    Status code;

    read_stdin_args(&argc, &argv);
    if (argc < 2 || read_flag(argv[1], &flag) != OK) {
        fprintf(stderr, "нужно: -q эпсилон a b c | -m число1 число2 | -t эпсилон a b c\n");
        return 1;
    }
    /** первый параметр (вещественное число) задаёт точность сравнения 
    вещественных чисел (эпсилон), оставшиеся три (вещественные числа) являются 
    коэффициентами квадратного уравнения; необходимо вывести в консоль решения 
    этого 
    уравнения 
    при 
    всевозможных 
    коэффициентов при степенях переменной; 
        - eps, 
        - a, 
        - b, 
        - c
    */
    if (flag == 'q') {
        double eps, a, b, c;
        Equation list[6];
        int count = 0;
        int index;
        if (argc != 6 || read_double(argv[2], &eps) || read_double(argv[3], &a) ||
            read_double(argv[4], &b) || read_double(argv[5], &c)) {
            fprintf(stderr, "для -q нужны эпсилон и три коэффициента\n");
            return 1;
        }
        code = solve_all_orders(eps, a, b, c, list, &count);
        if (code != OK) {
            fprintf(stderr, "некорректные данные\n");
            return 1;
        }
        for (index = 0; index < count; index++) {
            printf("уравнение: %.6g x^2 + %.6g x + %.6g = 0\n", list[index].first, list[index].second, list[index].third);
            if (list[index].root_count < 0) {
                printf("  бесконечно много корней\n");
            } else if (list[index].root_count == 0) {
                printf("  корней нет\n");
            } else if (list[index].root_count == 1) {
                printf("  x = %.6g\n", list[index].root1);
            } else {
                printf("  x1 = %.6g, x2 = %.6g\n", list[index].root1, list[index].root2);
            }
        }
        return 0;
    }
/** необходимо задать два ненулевых целых числа, после чего определить, кратно 
ли первое число второму; вывести в консоль соответствующее сообщение; 
    - left, 
    - right
*/
    if (flag == 'm') {
        long long left, right;
        int answer = 0;
        if (argc != 4 || read_long(argv[2], &left) || read_long(argv[3], &right)) {
            fprintf(stderr, "для -m нужны два целых числа\n");
            return 1;
        }
        code = is_multiple(left, right, &answer);
        if (code != OK) {
            fprintf(stderr, "числа должны быть ненулевыми\n");
            return 1;
        }
        printf("%lld %sкратно %lld\n", left, answer ? "" : "не ", right);
        return 0;
    }
/** первый параметр (вещественное число) задаёт точность сравнения вещественных 
чисел (эпсилон); необходимо проверить, могут ли оставшиеся три (вещественные 
числа) параметра являться длинами сторон прямоугольного треугольника. */
    if (flag == 't') {
        double eps, a, b, c;
        int answer = 0;
        if (argc != 6 || read_double(argv[2], &eps) || read_double(argv[3], &a) ||
            read_double(argv[4], &b) || read_double(argv[5], &c)) {
            fprintf(stderr, "для -t нужны эпсилон и три стороны\n");
            return 1;
        }
        
        code = is_right_triangle(eps, a, b, c, &answer);
        if (code != OK) {
            fprintf(stderr, "некорректные данные\n");
            return 1;
        }
        printf("%s\n", answer ? "прямоугольный" : "не прямоугольный");
        return 0;
    }

    fprintf(stderr, "флаг должен быть q, m или t\n");
    return 1;
}
