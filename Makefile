CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Wpedantic -g -O0
LDLIBS := -lm

.PHONY: all clean task1 task2 task3 task4 task5 task6 task7 task8 task9 task10

all: task1 task2 task3 task4 task5 task6 task7 task8 task9 task10

task1: task1/task1.exe
task2: task2/task2.exe
task3: task3/task3.exe
task4: task4/task4.exe
task5: task5/task5.exe
task6: task6/task6.exe
task7: task7/task7.exe
task8: task8/task8.exe
task9: task9/task9.exe
task10: task10/task10.exe

task1/task1.exe: task1/main.c task1/action.c task1/read.c task1/action.h task1/read.h task1/status.h
	$(CC) $(CFLAGS) -o $@ task1/main.c task1/action.c task1/read.c

task2/task2.exe: task2/main.c task2/calc.c task2/read.c task2/calc.h task2/read.h task2/status.h
	$(CC) $(CFLAGS) -o $@ task2/main.c task2/calc.c task2/read.c $(LDLIBS)

task3/task3.exe: task3/main.c task3/solve.c task3/read.c task3/solve.h task3/read.h task3/status.h
	$(CC) $(CFLAGS) -o $@ task3/main.c task3/solve.c task3/read.c $(LDLIBS)

task4/task4.exe: task4/main.c task4/edit.c task4/edit.h task4/status.h
	$(CC) $(CFLAGS) -o $@ task4/main.c task4/edit.c

task5/task5.exe: task5/main.c task5/calc.c task5/calc.h task5/status.h
	$(CC) $(CFLAGS) -o $@ task5/main.c task5/calc.c $(LDLIBS)

task6/task6.exe: task6/main.c task6/func.c task6/func.h task6/status.h
	$(CC) $(CFLAGS) -o $@ task6/main.c task6/func.c $(LDLIBS)

task7/task7.exe: task7/main.c task7/text.c task7/text.h task7/status.h
	$(CC) $(CFLAGS) -o $@ task7/main.c task7/text.c

task8/task8.exe: task8/main.c task8/convert.c task8/convert.h task8/status.h
	$(CC) $(CFLAGS) -o $@ task8/main.c task8/convert.c

task9/task9.exe: task9/main.c task9/array.c task9/array.h task9/status.h
	$(CC) $(CFLAGS) -o $@ task9/main.c task9/array.c

task10/task10.exe: task10/main.c task10/number.c task10/number.h task10/status.h
	$(CC) $(CFLAGS) -o $@ task10/main.c task10/number.c

clean:
	rm -f task1/task1.exe task2/task2.exe task3/task3.exe task4/task4.exe task5/task5.exe task6/task6.exe task7/task7.exe task8/task8.exe task9/task9.exe task10/task10.exe
