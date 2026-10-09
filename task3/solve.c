#include "solve.h"
#include <math.h>
#include <stddef.h>

// ---------------------------------------------
 
// good_eps принимает положительное конечное число погрешности
static int good_eps(const double eps) {
    return isfinite(eps) && eps > 0.0;
}

// close_to считает два числа равными, если модуль их разности не больше этой погрешности.
static int close_to(const double left, const double right, const double eps) {
    //** fabs из math.h возвращает модуль double: знак отбрасывается, само число остаётся. */
    return fabs(left - right) <= eps;
}

/*
 * solve_quadratic решает ax^2 + bx + c = 0.
 *
 * Два корне: x^2 - 5x + 6 = 0, то есть a = 1, b = -5, c = 6.
 *   D  = b^2 - 4ac = (-5)^2 - 4*1*6 = 25 - 24 = 1
 *   x1 = (-b - sqrt(D)) / (2a) = (5 - 1) / 2 = 2
 *   x2 = (-b + sqrt(D)) / (2a) = (5 + 1) / 2 = 3
 * Проверка: (x - 2)(x - 3) = x^2 - 5x + 6.
 * Здесь D > 0, поэтому root_count = 2, root1 = 2, root2 = 3.
 ------------------------------------------------------------
 * Один корень: x^2 - 4x + 4 = 0, то есть a = 1, b = -4, c = 4.
 *   D = (-4)^2 - 4*1*4 = 16 - 16 = 0
 *   x = -b / (2a) = 4 / 2 = 2
 * D = 0, поэтому root_count = 1, root1 = 2, root2 не используется.
-----------------------------------------------------------------
 * Бесконечно много корней: 0*x^2 + 0*x + 0 = 0, то есть a = 0, b = 0, c = 0.
 * Равенство 0 = 0 верно при любом x, конкретного корня нет.
 * root_count = -1, root1 и root2 не записываются.
 */
static void solve_quadratic(const double eps, const double a, const double b, const double c, Equation *item) {
    double disc; // результат расчета дискриминанта

    item->first  = a; // наши кожффициенты
    item->second = b; 
    item->third  = c;


    // root1 и root2 — это корни уравнения, то есть значения x, при которых 𝑎𝑥^2 + 𝑏𝑥 + 𝑐 = 0

    /** Сколько из них настоящие, показывает root_count:
      2 — два корня: root1 = (-b - √D) / (2a), root2 = (-b + √D) / (2a).
      1 — один корень, он лежит в root1. Так бывает у линейного уравнения -c / b и у квадратного с нулевым дискриминантом -b / (2a). root2 остаётся нулём и не используется.
      0 — корней нет, оба поля остаются нулями.
     -1 — решений бесконечно много, конкретные корни не записываются.
     **/

    item->root1 = 0.0; 
    item->root2 = 0.0;  // если корней нет, то root1 и root2 остаются 0.0

    // если a = 0, то уравнение линейное
    if (close_to(a, 0.0, eps)) {
        // если b = 0, то уравнение константное
        if (close_to(b, 0.0, eps)) {
            // если c = 0, то уравнение имеет бесконечное множество решений
            item->root_count = close_to(c, 0.0, eps) ? -1 : 0;
        } else {
            // если b != 0, то уравнение имеет одно решение
            item->root1 = -c / b;
            item->root_count = 1;
        }
        return; // нашли 1 решение
    }
    // если a != 0, то уравнение квадратное
    // вычисляем дискриминант D = b^2 - 4ac
    disc = b * b - 4.0 * a * c;
    // если дискриминант отрицательный, то корней нет
    if (disc < -eps) {
        item->root_count = 0; // корней нет
    } else if (fabs(disc) <= eps) { // если дискриминант равен 0, то уравнение имеет одно решение
        item->root1 = -b / (2.0 * a);
        item->root_count = 1; // одно решение
    } else { // если дискриминант положительный, то уравнение имеет два решения
        double root = sqrt(disc); // вычисляем корень из дискриминанта
        item->root1 = (-b - root) / (2.0 * a);
        item->root2 = (-b + root) / (2.0 * a); // вычисляем второй корень
        item->root_count = 2; // два решения
    }
}


/*
 * solve_all_orders переставляет коэффициенты a, b, c и решает каждое новое уравнение.
 *
 * Вызов: eps = 0.001, a = 1, b = 1, c = 2.
 * source[0] = 1, source[1] = 1, source[2] = 2, stored = 0.
 * Циклы оставляют только тройки индексов, где i, j, k все разные. Их шесть.
 *
 * 1) i=0, j=1, k=2 -> коэффициенты 1, 1, 2.
 *    Уравнение x^2 + x + 2 = 0.
 *    D = 1^2 - 4*1*2 = 1 - 8 = -7 < 0, корней нет.
 *    list[0]: root_count = 0. stored = 1.
 *
 * 2) i=0, j=2, k=1 -> коэффициенты 1, 2, 1.
 *    Уравнение x^2 + 2x + 1 = 0.
 *    D = 2^2 - 4*1*1 = 4 - 4 = 0.
 *    x = -2 / (2*1) = -1.
 *    list[1]: root_count = 1, root1 = -1. stored = 2.
 *
 * 3) i=1, j=0, k=2 -> коэффициенты 1, 1, 2.
 *    Такая тройка уже лежит в list[0], same = 1, уравнение пропускается.
 *
 * 4) i=1, j=2, k=0 -> коэффициенты 1, 2, 1.
 *    Совпадает с list[1], пропускается.
 *
 * 5) i=2, j=0, k=1 -> коэффициенты 2, 1, 1.
 *    Уравнение 2x^2 + x + 1 = 0.
 *    D = 1^2 - 4*2*1 = 1 - 8 = -7 < 0, корней нет.
 *    list[2]: root_count = 0. stored = 3.
 *
 * 6) i=2, j=1, k=0 -> коэффициенты 2, 1, 1.
 *    Совпадает с list[2], пропускается.
 *
 * В конце *count = 3. Из шести перестановок сохранены три разных уравнения.
 */
Status solve_all_orders(const double eps, const double a, const double b, const double c, Equation *list, int *count) {
    // храним исходны a,b,с
    double source[3];
    // индексы для прроходки от 0 до 2
    int i;
    int j;
    int k;
    // количество сохраненных решений уравнений
    int stored = 0;

    // проверяем входные данные
    if (!good_eps(eps) || list == NULL || count == NULL) {
        return BAD_ARG;
    }

    // созраняем исходные данные
    source[0] = a;
    source[1] = b;
    source[2] = c;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 3; k++) {
                // индексы для проверки на повторение
                int seen;
                // если в ранее решенном уравнении коэффициенты совпадают с текущими, то пропускаем
                int same;
                
                if (i == j || i == k || j == k) {
                    continue;
                }

                same = 0;
                // проверяем все ранее решенные уравнения на совпадение коэффициентов 
                for (seen = 0; seen < stored; seen++) {
                    if (close_to(list[seen].first, source[i], eps) &&
                        close_to(list[seen].second, source[j], eps) &&
                        close_to(list[seen].third, source[k], eps)) {
                        same = 1;
                    }
                }

                // если совпадение найдено, то пропускаем
                if (same) {
                    continue;
                }
                // решаем уравнение
                solve_quadratic(eps, source[i], source[j], source[k], &list[stored]);
                // увеличиваем количество сохраненных решений
                stored++;
            }
        }
    }
    *count = stored;
    return OK;
}

// ---------------------------------------------
// is_multiple проверяет, является ли первое число кратным второму  
Status is_multiple(const long long left, const long long right, int *answer) {
    if (answer == NULL || left == 0 || right == 0) {
        return BAD_ARG;
    }
    // если остаток от деления left на right равен 0, то left кратно right
    *answer = left % right == 0;
    return OK;
}

// ---------------------------------------------

Status is_right_triangle(const double eps, const double a, const double b, const double c, int *answer) {
    // проверяем входные данные
    // eps должен быть положительным конечным числом, а каждая сторона — длиннее eps. 
    // Иначе сторона слишком мала, чтобы быть стороной треугольника
    if (answer == NULL || !good_eps(eps) || a <= eps || b <= eps || c <= eps) {
        return BAD_ARG;
    }
    // проверяем все возможные варианты прямоугольного треугольника по теореме Пифагора
    // сумма квадратов катетов равна квадрату гипотенузы
    //  a^2 + b^2 = c^2
    //  a^2 + c^2 = b^2
    //  b^2 + c^2 = a^2
    // если хотя бы один из вариантов равен true, то треугольник прямоугольный
    // close_to(left, right, eps) - считает два числа равными, 
    // если модуль их разности не больше этой погрешности.
    *answer = close_to(a * a + b * b, c * c, eps) ||
              close_to(a * a + c * c, b * b, eps) ||
              close_to(b * b + c * c, a * a, eps);
    return OK;
}
