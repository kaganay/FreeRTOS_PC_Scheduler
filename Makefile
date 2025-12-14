# FreeRTOS PC Scheduler Makefile

all: derle

derle:
	gcc -c -I "./src" ./src/main.c -o ./src/main.o
	gcc -c -I "./src" ./src/scheduler.c -o ./src/scheduler.o
	gcc -c -I "./src" ./src/tasks.c -o ./src/tasks.o
	gcc -c -I "./src" ./src/freertos_hooks.c -o ./src/freertos_hooks.o
	gcc ./src/main.o ./src/scheduler.o ./src/tasks.o ./src/freertos_hooks.o -o freertos_sim.exe

calistir: derle
	./freertos_sim.exe giris.txt

clean:
	@if exist src\*.o del /q src\*.o
	@if exist freertos_sim.exe del /q freertos_sim.exe
	@echo Temizlik tamamlandi.

.PHONY: all derle calistir clean
