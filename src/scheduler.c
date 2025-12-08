#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Yeni bir kuyruk oluşturur
 */
Queue* queue_create(void) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for queue!\n");
        return NULL;
    }
    
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    
    return queue;
}

/**
 * Kuyruğu temizler
 * Not: Görevler tasks dizisinde tutulduğu için burada destroy edilmez
 */
void queue_destroy(Queue* queue) {
    if (queue == NULL) {
        return;
    }
    
    // Sadece kuyruk yapısını temizle, görevleri değil
    // Görevler tasks dizisinde tutuluyor
    free(queue);
}

/**
 * Kuyruğa görev ekler (FIFO)
 */
void queue_enqueue(Queue* queue, Task* task) {
    if (queue == NULL || task == NULL) {
        return;
    }
    
    task->next = NULL;
    
    if (queue->rear == NULL) {
        // Kuyruk boş
        queue->front = task;
        queue->rear = task;
    } else {
        // Kuyruğun sonuna ekle
        queue->rear->next = task;
        queue->rear = task;
    }
    
    queue->size++;
}

/**
 * Kuyruktan görev çıkarır (FIFO)
 */
Task* queue_dequeue(Queue* queue) {
    if (queue == NULL || queue->front == NULL) {
        return NULL;
    }
    
    Task* task = queue->front;
    queue->front = queue->front->next;
    
    if (queue->front == NULL) {
        // Kuyruk boşaldı
        queue->rear = NULL;
    }
    
    queue->size--;
    task->next = NULL;
    
    return task;
}

/**
 * Kuyruğun boş olup olmadığını kontrol eder
 */
bool queue_is_empty(Queue* queue) {
    return (queue == NULL || queue->front == NULL);
}

/**
 * Kuyruktaki görev sayısını döndürür
 */
int queue_size(Queue* queue) {
    if (queue == NULL) {
        return 0;
    }
    return queue->size;
}

/**
 * Giriş dosyasını parse eder ve görev listesi oluşturur
 */
int parse_input_file(const char* filename, Task*** tasks) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'!\n", filename);
        return -1;
    }
    
    // Önce satır sayısını say
    int task_count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Boş satırları ve yorum satırlarını atla
        if (line[0] == '\n' || line[0] == '#' || line[0] == '\r') {
            continue;
        }
        task_count++;
    }
    
    if (task_count == 0) {
        fclose(file);
        return 0;
    }
    
    // Görev listesi için bellek ayır
    *tasks = (Task**)malloc(task_count * sizeof(Task*));
    if (*tasks == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for task list!\n");
        fclose(file);
        return -1;
    }
    
    // Dosyayı başa al
    rewind(file);
    
    // Görevleri oku ve parse et
    int index = 0;
    int task_id = 1;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        // Boş satırları ve yorum satırlarını atla
        if (line[0] == '\n' || line[0] == '#' || line[0] == '\r') {
            continue;
        }
        
        // Satır sonu karakterlerini temizle
        line[strcspn(line, "\r\n")] = 0;
        
        // CSV formatını parse et: arrival_time,priority,burst_time
        int arrival_time, priority, burst_time;
        if (sscanf(line, "%d,%d,%d", &arrival_time, &priority, &burst_time) == 3) {
            // Öncelik kontrolü
            if (priority < 0 || priority > 3) {
                fprintf(stderr, "Warning: Task on line %d has invalid priority value (%d). Skipping.\n", 
                        index + 1, priority);
                continue;
            }
            
            // Görev oluştur
            Task* task = task_create(task_id++, priority, arrival_time, burst_time);
            if (task != NULL) {
                (*tasks)[index++] = task;
            }
        } else {
            fprintf(stderr, "Warning: Could not parse line %d: %s\n", index + 1, line);
        }
    }
    
    fclose(file);
    return index; // Gerçekte oluşturulan görev sayısı
}

/**
 * Görev listesini temizler
 */
void free_task_list(Task** tasks, int count) {
    if (tasks == NULL) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        task_destroy(tasks[i]);
    }
    
    free(tasks);
}

/**
 * Scheduler'ı başlatır ve gerekli kuyrukları oluşturur
 */
SchedulerState* scheduler_init(Task** tasks, int task_count) {
    (void)tasks;
    (void)task_count;
    SchedulerState* scheduler = (SchedulerState*)malloc(sizeof(SchedulerState));
    if (scheduler == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for scheduler!\n");
        return NULL;
    }
    
    scheduler->current_time = 0;
    scheduler->current_task = NULL;
    scheduler->total_tasks = task_count;
    scheduler->completed_tasks = 0;
    scheduler->task_list = NULL; // Bu görev listesi için kullanılmayacak
    
    // Gerçek zamanlı görev kuyruğu
    scheduler->rt_queue = queue_create();
    
    // 3 seviyeli kullanıcı görev kuyrukları
    for (int i = 0; i < 3; i++) {
        scheduler->user_queues[i] = queue_create();
    }
    
    return scheduler;
}

/**
 * Scheduler'ı temizler ve bellekleri serbest bırakır
 */
void scheduler_destroy(SchedulerState* scheduler) {
    if (scheduler == NULL) {
        return;
    }
    
    queue_destroy(scheduler->rt_queue);
    
    for (int i = 0; i < 3; i++) {
        queue_destroy(scheduler->user_queues[i]);
    }
    
    free(scheduler);
}

/**
 * Yeni gelen görevleri uygun kuyruklara ekler
 */
void scheduler_process_new_tasks(SchedulerState* scheduler, int current_time) {
    (void)scheduler;
    (void)current_time;
    // Bu fonksiyon main.c'de görev listesini kontrol edecek
    // Şimdilik placeholder
}

/**
 * Bir sonraki çalıştırılacak görevi seçer
 */
Task* scheduler_select_next_task(SchedulerState* scheduler) {
    if (scheduler == NULL) {
        return NULL;
    }
    
    // Önce gerçek zamanlı görevleri kontrol et
    if (!queue_is_empty(scheduler->rt_queue)) {
        return queue_dequeue(scheduler->rt_queue);
    }
    
    // Sonra kullanıcı görev kuyruklarını yüksek öncelikten düşüğe doğru kontrol et
    for (int i = 0; i < 3; i++) {
        if (!queue_is_empty(scheduler->user_queues[i])) {
            return queue_dequeue(scheduler->user_queues[i]);
        }
    }
    
    return NULL;
}

/**
 * Görevi çalıştırır (1 saniye veya tamamlanana kadar)
 * Not: Zaman artışı bu fonksiyon dışında yapılmalıdır
 */
void scheduler_execute_task(SchedulerState* scheduler, Task* task) {
    if (scheduler == NULL || task == NULL) {
        return;
    }
    
    // Eğer görev henüz başlamadıysa, başlat
    if (task->state != TASK_RUNNING) {
        task->state = TASK_RUNNING;
        scheduler->current_task = task;
        task->start_time = scheduler->current_time;  // Başlama zamanını kaydet
        
        // Görev başlangıç mesajı (görsel formatına uygun)
        printf("%s%.4f sn\t\ttask%d basladi\t(id:%04d) Oncelik:%d kalan sure:%d sn%s\n",
               task->color, (double)scheduler->current_time, task->id, task->id,
               task->current_priority, task->remaining_time, COLOR_RESET);
    }
    
    // Timeout kontrolü (20 saniye)
    if (task->start_time >= 0 && (scheduler->current_time - task->start_time) >= MAX_TASK_TIME) {
        printf("%s%.4f sn\t\ttask%d zamanasimi\t(id:%04d) Oncelik:%d kalan sure:%d sn%s\n",
               task->color, (double)scheduler->current_time, task->id, task->id,
               task->current_priority, task->remaining_time, COLOR_RESET);
        scheduler_complete_task(scheduler, task);
        return;
    }
    
    // Görev çalışma mesajı (görsel formatına uygun)
    printf("%s%.4f sn\t\ttask%d yurutuluyor\t(id:%04d) Oncelik:%d kalan sure:%d sn%s\n",
           task->color, (double)scheduler->current_time, task->id, task->id,
           task->current_priority, task->remaining_time, COLOR_RESET);
    
    // 1 saniye çalıştır (zaman kuantumu)
    task->remaining_time--;
    
    // Görev tamamlandı mı?
    if (task->remaining_time <= 0) {
        scheduler_complete_task(scheduler, task);
    }
}

/**
 * Mevcut görevi askıya alır ve önceliğini düşürür
 */
void scheduler_suspend_current_task(SchedulerState* scheduler) {
    if (scheduler == NULL || scheduler->current_task == NULL) {
        return;
    }
    
    Task* task = scheduler->current_task;
    task->state = TASK_SUSPENDED;
    
    // Askıya alma mesajı (görsel formatına uygun)
    int new_priority = (task->current_priority < 3) ? task->current_priority + 1 : 3;
    printf("%s%.4f sn\t\ttask%d askida\t\t(id:%04d) Oncelik:%d kalan sure:%d sn%s\n",
           task->color, (double)scheduler->current_time, task->id, task->id,
           new_priority, task->remaining_time, COLOR_RESET);
    
    // Önceliği düşür (geri besleme)
    if (task->current_priority < 3) {
        task->current_priority++;
    }
    
    // Uygun kuyruğa geri ekle
    int queue_index = task->current_priority - 1; // 1->0, 2->1, 3->2
    if (queue_index >= 0 && queue_index < 3) {
        queue_enqueue(scheduler->user_queues[queue_index], task);
    }
    
    scheduler->current_task = NULL;
}

/**
 * Görevi tamamlandı olarak işaretler
 * Not: Görev bellek alanı burada serbest bırakılmaz, 
 *      çünkü görev tasks dizisinde tutuluyor
 */
void scheduler_complete_task(SchedulerState* scheduler, Task* task) {
    if (scheduler == NULL || task == NULL) {
        return;
    }
    
    task->state = TASK_COMPLETED;
    
    // Tamamlanma mesajı (görsel formatına uygun)
    printf("%s%.4f sn\t\ttask%d sonlandi\t(id:%04d) Oncelik:%d kalan sure:0 sn%s\n",
           task->color, (double)scheduler->current_time, task->id, task->id,
           task->current_priority, COLOR_RESET);
    
    if (scheduler->current_task == task) {
        scheduler->current_task = NULL;
    }
    
    scheduler->completed_tasks++;
}


