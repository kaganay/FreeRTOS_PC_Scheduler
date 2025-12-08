@echo off
echo FreeRTOS PC Scheduler Derleniyor...

REM Include path'leri
set INCLUDES=-I./src -I./FreeRTOS/include -I./FreeRTOS/portable/ThirdParty/GCC/Posix -I./FreeRTOS
set CFLAGS=-Wall -Wextra -g -std=c11 %INCLUDES%

REM Proje kaynak dosyalarını derle
gcc %CFLAGS% -c src/main.c -o src/main.o
gcc %CFLAGS% -c src/scheduler.c -o src/scheduler.o
gcc %CFLAGS% -c src/tasks.c -o src/tasks.o
gcc %CFLAGS% -c src/freertos_hooks.c -o src/freertos_hooks.o

REM FreeRTOS çekirdek dosyalarını derle
gcc %CFLAGS% -c FreeRTOS/source/croutine.c -o FreeRTOS/source/croutine.o
gcc %CFLAGS% -c FreeRTOS/source/event_groups.c -o FreeRTOS/source/event_groups.o
gcc %CFLAGS% -c FreeRTOS/source/list.c -o FreeRTOS/source/list.o
gcc %CFLAGS% -c FreeRTOS/source/queue.c -o FreeRTOS/source/queue.o
gcc %CFLAGS% -c FreeRTOS/source/stream_buffer.c -o FreeRTOS/source/stream_buffer.o
gcc %CFLAGS% -c FreeRTOS/source/tasks.c -o FreeRTOS/source/tasks.o
gcc %CFLAGS% -c FreeRTOS/source/timers.c -o FreeRTOS/source/timers.o

REM POSIX port dosyasını derle
gcc %CFLAGS% -c FreeRTOS/portable/ThirdParty/GCC/Posix/port.c -o FreeRTOS/portable/ThirdParty/GCC/Posix/port.o
gcc %CFLAGS% -c FreeRTOS/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c -o FreeRTOS/portable/ThirdParty/GCC/Posix/utils/wait_for_event.o

REM Heap yöneticisi (heap_3 - malloc/free)
gcc %CFLAGS% -c FreeRTOS/portable/MemMang/heap_3.c -o FreeRTOS/portable/MemMang/heap_3.o

REM Tüm object dosyalarını linkle
gcc -Wall -Wextra -g -std=c11 src/main.o src/scheduler.o src/tasks.o src/freertos_hooks.o FreeRTOS/source/croutine.o FreeRTOS/source/event_groups.o FreeRTOS/source/list.o FreeRTOS/source/queue.o FreeRTOS/source/stream_buffer.o FreeRTOS/source/tasks.o FreeRTOS/source/timers.o FreeRTOS/portable/ThirdParty/GCC/Posix/port.o FreeRTOS/portable/ThirdParty/GCC/Posix/utils/wait_for_event.o FreeRTOS/portable/MemMang/heap_3.o -o freertos_sim.exe -lpthread

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Derleme basarili! Program: freertos_sim.exe
    echo.
    echo Calistirmak icin: freertos_sim.exe giris.txt
) else (
    echo.
    echo Derleme basarisiz!
)

pause

