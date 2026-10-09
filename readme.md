# Запуск задач через файл

Все команды выполняются из корня лабораторной в `cmd.exe`. Если параметров в командной строке нет, программа читает их одной строкой из стандартного ввода. Поэтому файл `args_taskNN_x.txt` подставляется так:

```
task1\task1.exe < task1\args_task01_a.txt
```

В PowerShell знак `<` занят. Та же команда оттуда:

```
cmd /c "task1\task1.exe < task1\args_task01_a.txt"
```

Задача 6 параметров не принимает, для неё файла нет. Задача 10 читает диалог с клавиатуры, её данные лежат в `input_task10_*.txt`.

Перед задачами 4, 7 и 8 должны лежать их входные тексты (`sample.txt`, `left.txt`, `numbers.txt` и остальные) в папке задачи.

## Задача 1

Файл — одна строка: число и флаг.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task1\args_task01_a.txt` | `10 -h` | `10 20 30 40 50 60 70 80 90 100` |
| `task1\args_task01_b.txt` | `-10 /h` | те же кратные, флаг через `/` |
| `task1\args_task01_c.txt` | `100 -h` | `100` |
| `task1\args_task01_d.txt` | `101 -h` | `Кратных нет` |
| `task1\args_task01_e.txt` | `0 -h` | код 1, число для флага не подходит |
| `task1\args_task01_f.txt` | `7 -p` | `Простое` |
| `task1\args_task01_g.txt` | `4 -p` | `Составное` |
| `task1\args_task01_h.txt` | `9 /p` | `Составное` |
| `task1\args_task01_i.txt` | `1 -p` | `Ни простое, ни составное` |
| `task1\args_task01_j.txt` | `255 -s` | `F F` |
| `task1\args_task01_k.txt` | `-255 /s` | `- F F` |
| `task1\args_task01_l.txt` | `0 -s` | `0` |
| `task1\args_task01_m.txt` | `2 -e` | таблица степеней, у основания 10 строка `10: 10 100` |
| `task1\args_task01_n.txt` | `1 /e` | столбец показателя 1: `1: 1` … `10: 10` |
| `task1\args_task01_o.txt` | `0 -e` | код 1, число не подходит |
| `task1\args_task01_p.txt` | `11 -e` | код 1, показатель больше 10 |
| `task1\args_task01_q.txt` | `10 -a` | `55` |
| `task1\args_task01_r.txt` | `1 /a` | `1` |
| `task1\args_task01_s.txt` | `5 -f` | `120` |
| `task1\args_task01_t.txt` | `0 /f` | `1` |
| `task1\args_task01_u.txt` | `21 -f` | код 2, `число не помещается` |
| `task1\args_task01_v.txt` | `-1 -f` | код 1, отрицательный факториал |

```
task1\task1.exe < task1\args_task01_a.txt
```

## Задача 2

Файл — положительный эпсилон. Печатаются 15 способов для e, π, ln 2, √2 и γ.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task2\args_task02_a.txt` | `0.001` | код 0, значения рядом с константами; у `gamma уравнение` пометка `(не сошлось)` |
| `task2\args_task02_b.txt` | `-0.001` | код 1, нужен положительный эпсилон |

```
task2\task2.exe < task2\args_task02_a.txt
```

## Задача 3

Файл — флаг и его числа.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task3\args_task03_a.txt` | `-q 0.001 1 -5 6` | 6 перестановок, первая пара корней `2` и `3` |
| `task3\args_task03_b.txt` | `/q 0.001 1 -4 4` | есть однокоренные строки `x = 2` и `x = 0.5` |
| `task3\args_task03_c.txt` | `-q 0.001 0 0 0` | `бесконечно много корней` |
| `task3\args_task03_d.txt` | `-q 0.001 1 1 2` | три уравнения: корней нет, `x = -1`, корней нет |
| `task3\args_task03_e.txt` | `-m 10 5` | `10 кратно 5` |
| `task3\args_task03_f.txt` | `/m 10 3` | `10 не кратно 3` |
| `task3\args_task03_g.txt` | `-m -10 5` | `-10 кратно 5` |
| `task3\args_task03_h.txt` | `-m 0 5` | код 1, числа должны быть ненулевыми |
| `task3\args_task03_i.txt` | `-t 0.001 3 4 5` | `прямоугольный` |
| `task3\args_task03_j.txt` | `/t 0.001 5 12 13` | `прямоугольный` |
| `task3\args_task03_k.txt` | `-t 0.001 2 3 4` | `не прямоугольный` |
| `task3\args_task03_l.txt` | `-x` | код 1, флаг должен быть q, m или t |

```
task3\task3.exe < task3\args_task03_a.txt
```

## Задача 4

Файл — флаг и пути. Без буквы `n` результат пишется рядом с входом под именем `out_...`. С буквой `n` путь выхода задан третьим параметром.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task4\args_task04_a.txt` | `-d task4/sample.txt` | `task4\out_sample.txt`: строки `abcd` и `Hi!` |
| `task4\args_task04_b.txt` | `/i task4/sample.txt` | `out_sample.txt`: `4` и `2` |
| `task4\args_task04_c.txt` | `-s task4/sample.txt` | `out_sample.txt`: `0` и `1` |
| `task4\args_task04_d.txt` | `/a task4/sample.txt` | `out_sample.txt`: `6162126364A486921A` |
| `task4\args_task04_e.txt` | `-nd task4/sample.txt task4/nd.txt` | `task4\nd.txt` как у `-d` |
| `task4\args_task04_f.txt` | `/ni task4/sample.txt task4/ni.txt` | `task4\ni.txt`: `4` и `2` |
| `task4\args_task04_g.txt` | `-ns task4/sample.txt task4/ns.txt` | `task4\ns.txt`: `0` и `1` |
| `task4\args_task04_h.txt` | `/na task4/sample.txt task4/na.txt` | `task4\na.txt`: та же hex-строка |
| `task4\args_task04_i.txt` | `-d task4/empty.txt` | пустой `task4\out_empty.txt` |
| `task4\args_task04_j.txt` | `-i task4/nonew.txt` | вход `Abc`, `task4\out_nonew.txt` содержит `3` |
| `task4\args_task04_k.txt` | `-s task4/crlf.txt` | вход `Ab` / `1!`, `task4\out_crlf.txt`: `0` и `1` |
| `task4\args_task04_l.txt` | `-d task4/nested/a.txt` | вход `12x`, `task4\nested\out_a.txt` содержит `x` |
| `task4\args_task04_m.txt` | `-d task4/no-such.txt` | код 1, входной файл не открылся |

```
task4\task4.exe < task4\args_task04_a.txt
```

## Задача 5

Файл — эпсилон и x.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task5\args_task05_a.txt` | `0.0001 0.5` | ряд a около `1.64872`, ряд b около `0.87758`, интеграл c около `1` |
| `task5\args_task05_b.txt` | `0.0001 1.5` | у рядов c и d текст `неверные данные`, остальные значения посчитаны |

```
task5\task5.exe < task5\args_task05_a.txt
```

## Задача 6

Параметров нет, файл аргументов не нужен.

```
task6\task6.exe
```

На экран выходит демонстрация: квадрат выпуклый, пятиугольник нет, многочлен в точке 2 равен `18`, числа Капрекара `0 1 45 297`, среднее геометрическое 4 и 9 равно `6`, `2^10 = 1024`, `2^(-3) = 0.125`, три корня около `1.4142135624`, `0.6931471806` и `1.5707963258`.

## Задача 7

Файл — флаг и пути к текстам.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task7\args_task07_a.txt` | `-r task7/left.txt task7/right.txt task7/merge.txt` | `task7\merge.txt`: `a x b y c` |
| `task7\args_task07_b.txt` | `/r task7/left.txt task7/right_long.txt task7/merge_long.txt` | `task7\merge_long.txt`: `a x b y c z w` |
| `task7\args_task07_c.txt` | `-a task7/tokens.txt task7/tokens_out.txt` | `task7\tokens_out.txt`: `A b C d 131131 f G h I 12201211123012301233` |
| `task7\args_task07_d.txt` | `/a task7/one.txt task7/one_out.txt` | `task7\one_out.txt`: `Hello` |
| `task7\args_task07_e.txt` | `-r task7/left.txt` | код 1, для `-r` нужны два входа и выход |

```
task7\task7.exe < task7\args_task07_a.txt
```

## Задача 8

Файл — входной и выходной пути.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task8\args_task08_a.txt` | `task8/numbers.txt task8/numbers_out.txt` | `task8\numbers_out.txt`: `FF 16 255`, `10 2 2`, `0 2 0`, `A 11 10`, `Z 36 35`, `101 2 5`, `A 11 10` |
| `task8\args_task08_b.txt` | `task8/bad.txt task8/bad_out.txt` | код 1, `плохая лексема` |
| `task8\args_task08_c.txt` | `task8/big.txt task8/big_out.txt` | код 2, `число не помещается` |

```
task8\task8.exe < task8\args_task08_a.txt
```

## Задача 9

Файл — границы `a b`. Числа в массиве случайные, проверяется правило, а не конкретный набор.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task9\args_task09_a.txt` | `1 5` | код 0: 20 чисел от 1 до 5, минимум и максимум меняются местами, длины A и B от 10 до 10000, в C столько же чисел, сколько в A |
| `task9\args_task09_b.txt` | `5 1` | код 1, нужно `a <= b` |

```
task9\task9.exe < task9\args_task09_a.txt
```

## Задача 10

Здесь файл — не строка параметров, а весь диалог: основание, числа и `Stop`.

| Файл | Содержимое | Результат |
| --- | --- | --- |
| `task10\input_task10_a.txt` | `10`, `15`, `-20`, `3`, `Stop` | максимум по модулю `-20` (в системах 9, 18, 27, 36: `-22`, `-12`, `-K`, `-K`), сумма `-2` |
| `task10\input_task10_b.txt` | `16`, `FF`, `-A`, `Stop` | максимум `255`, сумма `245` (в системе 36 сумма `6T`) |
| `task10\input_task10_c.txt` | `1` | код 1, основание должно быть от 2 до 36 |
| `task10\input_task10_d.txt` | `10`, `Stop` | код 1, чисел не было |
| `task10\input_task10_e.txt` | `16`, `ff`, `Stop` | код 1, буквы должны быть прописными |

```
task10\task10.exe < task10\input_task10_a.txt
```
