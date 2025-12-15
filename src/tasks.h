// Header guard: dosyanın birden fazla kez dahil edilmesini önler
#ifndef TASKS_H
#define TASKS_H

#include "scheduler.h"

/* Queue API */
// Kuyruk işlemleri: FIFO (First In First Out) kuyruk yönetimi
Queue* queue_create(void);              // Yeni kuyruk oluştur
void   queue_destroy(Queue* q);         // Kuyruğu temizle
void   queue_enqueue(Queue* q, Task* t); // Kuyruğa görev ekle
Task*  queue_dequeue(Queue* q);         // Kuyruktan görev çıkar
bool   queue_is_empty(Queue* q);        // Kuyruk boş mu kontrol et

/* Task + input */
// Görev oluşturma ve dosya okuma işlemleri
Task* task_create(int id, int priority, int arrival, int burst);  // Yeni görev oluştur
int   parse_input_file(const char* filename, Task*** tasks_out);   // Dosyadan görevleri oku
void  free_task_list(Task** tasks, int count);                     // Görev listesini temizle

#endif
