# FreeRTOS PC Scheduler Makefile

# Derleyici
CC = gcc

# Derleme bayrakları
CONFIG_DEFS = -DprojCOVERAGE_TEST=0 -DprojENABLE_TRACING=0 -D_GNU_SOURCE
CFLAGS = -Wall -Wextra -g -std=c11 $(CONFIG_DEFS)
INCLUDES = -I./src -I./FreeRTOS/include -I./FreeRTOS/portable/ThirdParty/GCC/Posix -I./FreeRTOS

# Proje kaynak dosyalar
SRC_DIR = src
PROJECT_SOURCES = $(SRC_DIR)/main.c \
                  $(SRC_DIR)/scheduler.c \
                  $(SRC_DIR)/tasks.c \
                  $(SRC_DIR)/freertos_hooks.c

# FreeRTOS çekirdek dosyaları
FREERTOS_SOURCES = FreeRTOS/source/croutine.c \
                    FreeRTOS/source/event_groups.c \
                    FreeRTOS/source/list.c \
                    FreeRTOS/source/queue.c \
                    FreeRTOS/source/stream_buffer.c \
                    FreeRTOS/source/tasks.c \
                    FreeRTOS/source/timers.c \
                    FreeRTOS/portable/MemMang/heap_3.c

# POSIX port dosyası
POSIX_SOURCES = FreeRTOS/portable/ThirdParty/GCC/Posix/port.c \
                FreeRTOS/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c

# Tüm kaynak dosyalar
SOURCES = $(PROJECT_SOURCES) $(FREERTOS_SOURCES) $(POSIX_SOURCES)

# Object dosyalar
OBJECTS = $(SOURCES:.c=.o)

# Hedef program
TARGET = freertos_sim

# Varsayılan hedef
all: $(TARGET)

# Programı derle
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) -lpthread

# Proje kaynak dosyalarını derle
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# FreeRTOS çekirdek dosyalarını derle
FreeRTOS/source/%.o: FreeRTOS/source/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# POSIX port dosyasını derle
FreeRTOS/portable/ThirdParty/GCC/Posix/%.o: FreeRTOS/portable/ThirdParty/GCC/Posix/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Heap (MemMang) derleme kuralı
FreeRTOS/portable/MemMang/%.o: FreeRTOS/portable/MemMang/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Temizlik
clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).exe
	@echo "Temizlik tamamlandı."

# Test çalıştırma
test: $(TARGET)
	./$(TARGET) giris.txt

# Yardım
help:
	@echo "Kullanılabilir hedefler:"
	@echo "  make        - Projeyi derle"
	@echo "  make clean  - Derleme dosyalarını temizle"
	@echo "  make test   - Programı test et"
	@echo "  make help   - Bu yardım mesajını göster"

.PHONY: all clean test help

