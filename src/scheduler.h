#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

// Renk kodları için ANSI escape kodları
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

// Öncelik seviyeleri
#define PRIORITY_RT       0  // Gerçek zamanlı (Real-Time)
#define PRIORITY_HIGH     1  // Yüksek öncelikli kullanıcı görevi
#define PRIORITY_MEDIUM   2  // Orta öncelikli kullanıcı görevi
#define PRIORITY_LOW      3  // Düşük öncelikli kullanıcı görevi

// Maksimum görev süresi (saniye)
#define MAX_TASK_TIME     20

// Zaman kuantumu (saniye)
#define TIME_QUANTUM      1

// Toplam öncelik seviyesi sayısı (RT + 3 kullanıcı seviyesi)
#define NUM_PRIORITY_LEVELS 4

// Görev durumları
typedef enum {
    TASK_READY,      // Hazır
    TASK_RUNNING,    // Çalışıyor
    TASK_SUSPENDED,  // Askıya alınmış
    TASK_COMPLETED   // Tamamlandı
} TaskState;

// Görev yapısı
typedef struct Task {
    int id;                    // Görev kimliği
    int priority;              // Öncelik seviyesi (0=RT, 1-3=kullanıcı)
    int arrival_time;          // Varış zamanı
    int burst_time;            // Toplam çalışma süresi
    int remaining_time;         // Kalan çalışma süresi
    int current_priority;      // Mevcut öncelik seviyesi (geri besleme için)
    TaskState state;           // Görev durumu
    const char* color;         // Çıktı rengi
    int start_time;            // Görevin başlama zamanı (timeout kontrolü için)
    struct Task* next;         // Bağlı liste için
} Task;

// Kuyruk yapısı (FIFO için)
typedef struct Queue {
    Task* front;               // Kuyruğun başı
    Task* rear;                // Kuyruğun sonu
    int size;                  // Kuyruktaki görev sayısı
} Queue;

// Scheduler durumu
typedef struct SchedulerState {
    int current_time;          // Mevcut zaman (tick)
    Task* current_task;        // Şu anda çalışan görev
    Queue* rt_queue;           // Gerçek zamanlı görev kuyruğu
    Queue* user_queues[3];     // 3 seviyeli kullanıcı görev kuyrukları
    Task* task_list;           // Tüm görevlerin listesi (giriş dosyasından)
    int total_tasks;           // Toplam görev sayısı
    int completed_tasks;       // Tamamlanan görev sayısı
} SchedulerState;

// Fonksiyon bildirimleri

// Kuyruk işlemleri
Queue* queue_create(void);
void queue_destroy(Queue* queue);
void queue_enqueue(Queue* queue, Task* task);
Task* queue_dequeue(Queue* queue);
bool queue_is_empty(Queue* queue);
int queue_size(Queue* queue);

// Görev işlemleri
Task* task_create(int id, int priority, int arrival_time, int burst_time);
void task_destroy(Task* task);
const char* task_get_color(int task_id);
void task_print_info(Task* task, const char* message);

// Giriş dosyası işlemleri
int parse_input_file(const char* filename, Task*** tasks);
void free_task_list(Task** tasks, int count);

// Scheduler işlemleri
SchedulerState* scheduler_init(Task** tasks, int task_count);
void scheduler_destroy(SchedulerState* scheduler);
void scheduler_process_new_tasks(SchedulerState* scheduler, int current_time);
Task* scheduler_select_next_task(SchedulerState* scheduler);
void scheduler_execute_task(SchedulerState* scheduler, Task* task);
void scheduler_suspend_current_task(SchedulerState* scheduler);
void scheduler_complete_task(SchedulerState* scheduler, Task* task);

#endif // SCHEDULER_H

