# ============================================================
# FreeRTOS PC Scheduler - Makefile (Windows + Git Bash + Linux)
# ============================================================

# ------------------------------------------------------------
# 1) Ortam tespiti
#    - Git Bash/MSYS/MINGW: uname -s içinde MINGW/MSYS/CYGWIN geçer
#    - CMD/PowerShell: OS=Windows_NT ama uname yok / farklı olabilir
# ------------------------------------------------------------
UNAME_S := $(shell uname -s 2>/dev/null)

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	ENV := GITBASH
else ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
	ENV := GITBASH
else ifeq ($(findstring CYGWIN,$(UNAME_S)),CYGWIN)
	ENV := GITBASH
else ifeq ($(OS),Windows_NT)
	ENV := WINDOWS
else
	ENV := LINUX
endif

# ------------------------------------------------------------
# 2) Araçlar / hedef adı
# ------------------------------------------------------------
CC := gcc
CFLAGS := -Wall -Wextra -O2 -g

INCLUDES := \
	-I"./src" \
	-I"./FreeRTOS" \
	-I"./FreeRTOS/include" \
	-I"./FreeRTOS/portable/ThirdParty/GCC/Posix"

DEFINES := -DprojCOVERAGE_TEST=0 -DprojENABLE_TRACING=0

# Windows'ta gerçek pthread.h varsa HAVE_PTHREAD_H=1
ifeq ($(ENV),WINDOWS)
	ifneq ($(wildcard C:/MinGW/include/pthread.h),)
		DEFINES += -DHAVE_PTHREAD_H=1
	endif
endif

# Uzantısız hedef: Windows'ta bile "freertos_sim" üretelim
TARGET := freertos_sim
INPUT  := giris.txt

# Clean komutu ortam bazlı
ifeq ($(ENV),WINDOWS)
	RM := del /q
else
	RM := rm -f
endif

# ------------------------------------------------------------
# 3) Kaynak dosyalar
# ------------------------------------------------------------
FREERTOS_SRC := \
	./FreeRTOS/source/tasks.c \
	./FreeRTOS/source/queue.c \
	./FreeRTOS/source/list.c \
	./FreeRTOS/source/timers.c \
	./FreeRTOS/source/event_groups.c \
	./FreeRTOS/source/stream_buffer.c \
	./FreeRTOS/source/croutine.c \
	./FreeRTOS/portable/MemMang/heap_3.c \
	./FreeRTOS/portable/ThirdParty/GCC/Posix/port.c \
	./FreeRTOS/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c

APP_SRC := \
	./src/main.c \
	./src/scheduler.c \
	./src/tasks.c \
	./src/freertos_hooks.c

FREERTOS_OBJ := $(FREERTOS_SRC:.c=.o)
APP_OBJ := $(APP_SRC:.c=.o)
ALL_OBJ := $(FREERTOS_OBJ) $(APP_OBJ)

# ------------------------------------------------------------
# 4) Varsayılan hedef
# ------------------------------------------------------------
all: $(TARGET)
	@echo
	@echo "================================================="
	@echo "Build complete: $(TARGET)"
	@echo "================================================="
ifeq ($(ENV),WINDOWS)
	@echo "Calistirma Ortami : Windows (CMD / PowerShell)"
	@echo
	@echo "  Dogru komut:"
	@echo "    .\\freertos_sim $(INPUT)"
	@echo
	@echo "  Bilgi:"
	@echo "    Windows kabuklari (CMD/PowerShell) Linux'taki"
	@echo "    './program' sozdizimini desteklemez."
	@echo "    Bu nedenle '.\\program' kullanilmalidir."
else
	@echo "Calistirma Ortami : Linux / WSL / Git Bash"
	@echo
	@echo "  Dogru komut:"
	@echo "    ./freertos_sim $(INPUT)"
	@echo
	@echo "  Bilgi:"
	@echo "    Bu ortamlar POSIX uyumludur ve './program'"
	@known: @echo "    seklinde calistirma desteklenir."
endif
	@echo "================================================="

# ------------------------------------------------------------
# 5) Link
# ------------------------------------------------------------
$(TARGET): $(ALL_OBJ)
ifeq ($(ENV),LINUX)
	$(CC) $(ALL_OBJ) -o $(TARGET) -lpthread
else
	$(CC) $(ALL_OBJ) -o $(TARGET)
endif

# ------------------------------------------------------------
# 6) Compile kuralları
# ------------------------------------------------------------
./FreeRTOS/source/%.o: ./FreeRTOS/source/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

./FreeRTOS/portable/MemMang/%.o: ./FreeRTOS/portable/MemMang/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

./FreeRTOS/portable/ThirdParty/GCC/Posix/%.o: ./FreeRTOS/portable/ThirdParty/GCC/Posix/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

./FreeRTOS/portable/ThirdParty/GCC/Posix/utils/%.o: ./FreeRTOS/portable/ThirdParty/GCC/Posix/utils/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

./src/%.o: ./src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# ------------------------------------------------------------
# 7) Çalıştırma hedefleri
# ------------------------------------------------------------
run: $(TARGET)
ifeq ($(ENV),WINDOWS)
	.\$(TARGET) $(INPUT)
else
	./$(TARGET) $(INPUT)
endif

test: run

# ------------------------------------------------------------
# 8) Temizlik
# ------------------------------------------------------------
clean:
ifeq ($(ENV),WINDOWS)
	@if exist src\*.o $(RM) src\*.o
	@if exist FreeRTOS\source\*.o $(RM) FreeRTOS\source\*.o
	@if exist FreeRTOS\portable\MemMang\*.o $(RM) FreeRTOS\portable\MemMang\*.o
	@if exist FreeRTOS\portable\ThirdParty\GCC\Posix\*.o $(RM) FreeRTOS\portable\ThirdParty\GCC\Posix\*.o
	@if exist FreeRTOS\portable\ThirdParty\GCC\Posix\utils\*.o $(RM) FreeRTOS\portable\ThirdParty\GCC\Posix\utils\*.o
	@if exist $(TARGET) $(RM) $(TARGET)
else
	$(RM) $(ALL_OBJ) $(TARGET)
endif
	@echo "Temizlik tamamlandi."

.PHONY: all run test clean
