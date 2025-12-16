// FreeRTOS kullanarak görev ve kuyruk işlemleri
// Proje gereksinimlerine göre: FreeRTOS bellek yönetimi ve API'leri kullanılır

#include "tasks.h"      // Task ve Queue fonksiyon prototipleri
#include "FreeRTOS.h"   // FreeRTOS bellek yönetimi (pvPortMalloc/vPortFree)
#include "task.h"       // FreeRTOS task tipleri
#include <stdio.h>      // Dosya işlemleri, printf
#include <string.h>     // strlen, sscanf vb.

/* KIRMIZI YOK: normal prosesler asla kirmizi olmaz */
// Görev ID'sine göre renk atar (kırmızı hariç - sadece timeout için)
static const char* get_color_by_id(int id) {
    // Mevcut renk paleti
    const char* colors[] = { // Sırasıyla kullanılacak renk listesi
        COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
        COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
    };
    // ID'ye göre modüler renk seçimi
    return colors[id % 6];
}

// Yeni bir görev oluşturur ve başlangıç değerlerini ayarlar
// FreeRTOS bellek yönetimi kullanılır
Task* task_create(int id, int priority, int arrival, int burst) {
    // FreeRTOS bellek yönetimi ile görev için yer ayır
    Task* t = (Task*)pvPortMalloc(sizeof(Task)); // FreeRTOS heap'inden bellek al
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
    t->start_time = -1;        // Henüz CPU almamış
    t->last_run_time = -1;     // Henüz CPU almamış

    // Göreve renk ata
    t->color = get_color_by_id(id);
    t->next = NULL;            // Bağlı liste için NULL
    return t;
}

// Girdi dosyasını okuyup görev listesi oluşturur
// Format: arrival_time, priority, burst_time
// FreeRTOS bellek yönetimi kullanılır
int parse_input_file(const char* filename, Task*** tasks_out) {
    // Dosyayı aç
    FILE* f = fopen(filename, "r"); // Girdi dosyasını aç
    if (!f) {
        perror("Dosya acilamadi");
        return 0;
    }

    // Önce satır sayısını say (bellek ayırmak için)
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f)) { // Satır satır oku
        if (strlen(line) > 2) count++;  // Boş satırları atla
    }
    rewind(f);  // Dosyayı başa sar (ikinci kez okumak için)

    // FreeRTOS bellek yönetimi ile görev pointer'ları için dizi ayır
    Task** arr = (Task**)pvPortMalloc(sizeof(Task*) * count); // Görev pointer dizisi
    if (!arr) { fclose(f); return 0; }

    // Dosyayı oku ve görevleri oluştur
    int idx = 0;
    int id = 0;
    int a,p,b;  // arrival, priority, burst

    while (fgets(line, sizeof(line), f)) { // Her satır için parse dene
        // Satırı parse et: "arrival, priority, burst" formatında
        if (sscanf(line, " %d , %d , %d", &a, &p, &b) == 3) { // 3 değer başarıyla okunduysa
            arr[idx++] = task_create(id++, p, a, b);
        }
    }

    // Dosyayı kapat ve sonuçları döndür
    fclose(f); // Dosyayı kapat
    *tasks_out = arr;
    return idx;  // Görev sayısını döndür
}

// Görev listesini ve tüm görevleri bellekten temizler
// FreeRTOS bellek yönetimi kullanılır
void free_task_list(Task** tasks, int count) {
    // Her görevi ayrı ayrı serbest bırak (FreeRTOS bellek yönetimi)
    for (int i = 0; i < count; i++) {
        if (tasks[i]) {
            vPortFree(tasks[i]);
        }
    }
    // Görev pointer dizisini serbest bırak
    vPortFree(tasks);
}

/* Queue - FreeRTOS queue'ları kullanılacak ama kendi Queue yapımız da gerekli */
// Yeni bir kuyruk oluşturur ve başlatır
// FreeRTOS bellek yönetimi kullanılır
Queue* queue_create(void) {
    // FreeRTOS bellek yönetimi ile kuyruk için yer ayır
    Queue* q = (Queue*)pvPortMalloc(sizeof(Queue));
    if (!q) return NULL;
    // Başlangıçta kuyruk boş
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}

// Kuyruğu bellekten temizler (not: kuyruktaki görevler ayrı temizlenmeli)
// FreeRTOS bellek yönetimi kullanılır
void queue_destroy(Queue* q) {
    if (q) {
        vPortFree(q);
    }
}

// Kuyruğun sonuna görev ekler (FIFO)
void queue_enqueue(Queue* q, Task* t) {
    if (!t || !q) return;  // Geçersiz görev/kuyruk kontrolü
    t->next = NULL;  // Yeni elemanın next'ini NULL yap
    // Kuyruk boş değilse son elemana bağla, boşsa hem front hem rear'ı ayarla
    if (q->rear) q->rear->next = t;
    else q->front = t;
    q->rear = t;     // Yeni son eleman
    q->size++;       // Boyutu artır
}

// Kuyruğun başından görev çıkarır (FIFO)
Task* queue_dequeue(Queue* q) {
    if (!q || !q->front) return NULL;  // Kuyruk boşsa NULL döndür
    Task* t = q->front;           // İlk elemanı al
    q->front = t->next;           // Front'u bir sonraki elemana taşı
    if (!q->front) q->rear = NULL; // Kuyruk boşaldıysa rear'ı da NULL yap
    t->next = NULL;               // Çıkarılan elemanın next'ini temizle
    q->size--;                     // Boyutu azalt
    return t;                      // Çıkarılan görevi döndür
}

// Kuyruğun boş olup olmadığını kontrol eder
bool queue_is_empty(Queue* q) {
    if (!q) return true;
    return q->front == NULL;  // Front NULL ise kuyruk boştur
}
