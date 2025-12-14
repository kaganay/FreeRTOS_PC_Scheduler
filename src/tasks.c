// Gerekli kütüphaneleri dahil et
#include "tasks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* KIRMIZI YOK: normal prosesler asla kirmizi olmaz */
// Görev ID'sine göre renk atar (kırmızı hariç - sadece timeout için)
static const char* get_color_by_id(int id) {
    // Mevcut renk paleti
    const char* colors[] = {
        COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
        COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
    };
    // ID'ye göre modüler renk seçimi
    return colors[id % 6];
}

// Yeni bir görev oluşturur ve başlangıç değerlerini ayarlar
Task* task_create(int id, int priority, int arrival, int burst) {
    // Bellekte görev için yer ayır
    Task* t = (Task*)malloc(sizeof(Task));
    if (!t) return NULL;

    // Görev bilgilerini başlat
    t->id = id;
    t->priority = priority;
    t->current_priority = priority;  // Başlangıçta öncelikler aynı
    t->arrival_time = arrival;
    t->burst_time = burst;
    t->remaining_time = burst;       // Başlangıçta kalan süre = toplam süre
    t->state = TASK_READY;            // Başlangıç durumu: hazır

    /* timeout icin gerekli init */
    // Zaman aşımı kontrolü için zaman bilgilerini başlat
    t->start_time = -1;        // Henüz çalışmadı
    t->last_run_time = -1;     // Henüz çalışmadı

    // Göreve renk ata
    t->color = get_color_by_id(id);
    t->next = NULL;            // Bağlı liste için NULL
    return t;
}

// Girdi dosyasını okuyup görev listesi oluşturur
// Format: arrival_time, priority, burst_time
int parse_input_file(const char* filename, Task*** tasks_out) {
    // Dosyayı aç
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Dosya acilamadi");
        return 0;
    }

    // Önce satır sayısını say (bellek ayırmak için)
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strlen(line) > 2) count++;  // Boş satırları atla
    }
    rewind(f);  // Dosyayı başa sar

    // Görev pointer'ları için dizi ayır
    Task** arr = (Task**)malloc(sizeof(Task*) * count);
    if (!arr) { fclose(f); return 0; }

    // Dosyayı oku ve görevleri oluştur
    int idx = 0;
    int id = 0;
    int a,p,b;  // arrival, priority, burst

    while (fgets(line, sizeof(line), f)) {
        // Satırı parse et: "arrival, priority, burst" formatında
        if (sscanf(line, " %d , %d , %d", &a, &p, &b) == 3) {
            arr[idx++] = task_create(id++, p, a, b);
        }
    }

    // Dosyayı kapat ve sonuçları döndür
    fclose(f);
    *tasks_out = arr;
    return idx;  // Görev sayısını döndür
}

// Görev listesini ve tüm görevleri bellekten temizler
void free_task_list(Task** tasks, int count) {
    // Her görevi ayrı ayrı serbest bırak
    for (int i = 0; i < count; i++) free(tasks[i]);
    // Görev pointer dizisini serbest bırak
    free(tasks);
}

/* Queue */
// Yeni bir kuyruk oluşturur ve başlatır
Queue* queue_create(void) {
    // Kuyruk için bellek ayır
    Queue* q = (Queue*)malloc(sizeof(Queue));
    // Başlangıçta kuyruk boş
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}

// Kuyruğu bellekten temizler (not: kuyruktaki görevler ayrı temizlenmeli)
void queue_destroy(Queue* q) {
    free(q);
}

// Kuyruğun sonuna görev ekler (FIFO)
void queue_enqueue(Queue* q, Task* t) {
    if (!t) return;  // Geçersiz görev kontrolü
    t->next = NULL;  // Yeni elemanın next'ini NULL yap
    // Kuyruk boş değilse son elemana bağla, boşsa hem front hem rear'ı ayarla
    if (q->rear) q->rear->next = t;
    else q->front = t;
    q->rear = t;     // Yeni son eleman
    q->size++;       // Boyutu artır
}

// Kuyruğun başından görev çıkarır (FIFO)
Task* queue_dequeue(Queue* q) {
    if (!q->front) return NULL;  // Kuyruk boşsa NULL döndür
    Task* t = q->front;           // İlk elemanı al
    q->front = t->next;           // Front'u bir sonraki elemana taşı
    if (!q->front) q->rear = NULL; // Kuyruk boşaldıysa rear'ı da NULL yap
    t->next = NULL;               // Çıkarılan elemanın next'ini temizle
    q->size--;                     // Boyutu azalt
    return t;                      // Çıkarılan görevi döndür
}

// Kuyruğun boş olup olmadığını kontrol eder
bool queue_is_empty(Queue* q) {
    return q->front == NULL;  // Front NULL ise kuyruk boştur
}
