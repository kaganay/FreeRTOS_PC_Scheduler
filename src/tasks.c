#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * Görev için rastgele renk seçer
 * Her görev ID'si için tutarlı bir renk döndürür
 */
const char* task_get_color(int task_id) {
    const char* colors[] = {
        COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
        COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
    };
    
    // Görev ID'sine göre deterministik renk seçimi
    return colors[task_id % 7];
}

/**
 * Yeni bir görev oluşturur
 */
Task* task_create(int id, int priority, int arrival_time, int burst_time) {
    Task* task = (Task*)malloc(sizeof(Task));
    if (task == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for task!\n");
        return NULL;
    }
    
    task->id = id;
    task->priority = priority;
    task->arrival_time = arrival_time;
    task->burst_time = burst_time;
    task->remaining_time = burst_time;
    task->current_priority = priority;
    task->state = TASK_READY;
    task->color = task_get_color(id);
    task->start_time = -1;  // Henüz başlamadı
    task->next = NULL;
    
    return task;
}

/**
 * Görev bellek alanını serbest bırakır
 */
void task_destroy(Task* task) {
    if (task != NULL) {
        free(task);
    }
}

/**
 * Görev bilgilerini renkli olarak yazdırır
 */
void task_print_info(Task* task, const char* message) {
    if (task == NULL) {
        return;
    }
    
    printf("%s[Zaman: %d] Görev %d (Öncelik: %d, Kalan: %d saniye) - %s%s\n",
           task->color,
           task->arrival_time, // Bu zaman scheduler'dan gelecek
           task->id,
           task->current_priority,
           task->remaining_time,
           message,
           COLOR_RESET);
}

