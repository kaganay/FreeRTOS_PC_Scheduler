// FreeRTOS kullanarak 4 seviyeli öncelikli scheduler implementasyonu
// Proje gereksinimlerine göre: FreeRTOS çekirdeği tam entegre kullanılır
// Scheduler mantığı (öncelik seçimi, MLFQ) kendi kodumuzda implement edilmiştir

#include "scheduler.h"
#include "tasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>

/* Son yazdırılan rengi ve ID'yi takip etmek için statik değişkenler */
static const char* last_printed_color = NULL;
static int last_printed_id = -1;

/* FreeRTOS semaphore - thread-safe printing için */
static SemaphoreHandle_t g_print_mutex = NULL;

/* Queue'dan geçerli (timeout / completed olmayan) görev çıkar */
static Task* dequeue_valid(Queue* q) {
    while (!queue_is_empty(q)) {
        Task* t = queue_dequeue(q);
        if (!t) return NULL;
        if (t->state != TASK_TIMEOUT && t->state != TASK_COMPLETED) {
            return t;
        }
    }
    return NULL;
}

/* Normal loglar - FreeRTOS semaphore ile thread-safe (birden fazla thread aynı anda yazmaz) */
static void print_log(double time, const char* status, Task* t) {
    // FreeRTOS semaphore ile thread-safe yazdırma
    if (g_print_mutex) {
        xSemaphoreTake(g_print_mutex, portMAX_DELAY);
    }
    
    const char* color_to_use = t->color;      // Varsayılan renk: görevin rengi
    if (t->id == 16) color_to_use = COLOR_WHITE; // Özel durum: ID 16 için beyaz renk
    if (strcmp(status, "zamanasimi") == 0) {
        color_to_use = COLOR_RED;
    } else {
        // Arka arkaya aynı renkte farklı ID gözükmesin diye ufak renk ayarı
        if (last_printed_color != NULL && last_printed_id != t->id) {
            if (color_to_use == last_printed_color) {
                color_to_use = COLOR_WHITE;
                if (color_to_use == last_printed_color) {
                    color_to_use = COLOR_CYAN;
                }
                if (t->id == 16) color_to_use = COLOR_WHITE;
            }
        }
    }
    
    // Zaman, durum, görev ID, öncelik ve kalan süreyi tek satırda yazdır
    printf("%s%.4f sn\tproses %-15s\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s\n",
           color_to_use, time, status, t->id, t->current_priority, t->remaining_time, COLOR_RESET);
    
    last_printed_color = color_to_use;
    last_printed_id = t->id;
    
    if (g_print_mutex) {
        xSemaphoreGive(g_print_mutex);
    }
}

/* Zaman aşımı logu - FreeRTOS semaphore ile thread-safe */
static void print_timeout(double time, Task* t) {
    if (g_print_mutex) {
        xSemaphoreTake(g_print_mutex, portMAX_DELAY);
    }
    
    char buf[24];
    snprintf(buf, sizeof(buf), "%-15s", "zamanasimi");
    printf(COLOR_RED "%.4f sn\tproses %s\t(id:%04d\toncelik:%d\tkalan sure:%d sn)" COLOR_RESET "\n",
           time, buf, t->id, t->current_priority, t->remaining_time);
    
    last_printed_color = COLOR_RED;
    last_printed_id = t->id;
    
    if (g_print_mutex) {
        xSemaphoreGive(g_print_mutex);
    }
}

/* Timeout kontrolü: 20 sn boyunca CPU alamayan READY/SUSPENDED görevleri TIMEOUT yapar */
static void check_timeouts(int now, Task** all, int n, Task* running, int* doneCounter) {
    for (int i = 0; i < n; i++) {
        Task* x = all[i];
        if (x->state == TASK_COMPLETED || x->state == TASK_TIMEOUT) continue; // Zaten bitti / timeout
        if (x == running) continue;                                           // Şu an çalışanı kontrol etme
        if (x->arrival_time > now) continue;                                  // Henüz sisteme girmemiş
        if (!(x->state == TASK_READY || x->state == TASK_SUSPENDED)) continue; // Sadece READY/SUSPENDED
        
        // Başlamadıysa arrival, başladıysa son çalıştığı zaman referans alınır
        int ref_time = (x->start_time == -1) ? x->arrival_time : x->last_run_time;
        if ((now - ref_time) >= 20) {
            x->state = TASK_TIMEOUT;
            print_timeout((double)now, x);
            (*doneCounter)++;
        }
    }
}

/* Ana simülasyon fonksiyonu - FreeRTOS kullanarak 4 seviyeli scheduler */
void run_simulation(const char* filename) {
    // Girdi dosyasından görevleri oku (FreeRTOS bellek yönetimi kullanılır)
    Task** all = NULL;                       // Tüm görevlerin tutulacağı dizi
    int n = parse_input_file(filename, &all); // Girdi dosyasını oku
    if (n <= 0) return;
    
    // FreeRTOS semaphore oluştur (thread-safe printing için)
    g_print_mutex = xSemaphoreCreateMutex();
    
    // Öncelik seviyelerine göre kuyruklar oluştur (FreeRTOS bellek yönetimi ile)
    Queue* rt = queue_create();  /* prio 0 - Real-time (FCFS) */
    Queue* q1 = queue_create();   /* prio 1 - Yüksek öncelik */
    Queue* q2 = queue_create();   /* prio 2 - Orta öncelik */
    Queue* q3 = queue_create();   /* prio 3 - Düşük öncelik (Round-Robin) */
    
    if (!rt || !q1 || !q2 || !q3) {
        printf("Hata: Kuyruklar olusturulamadi!\n");
        free_task_list(all, n);
        return;
    }
    
    // Simülasyon değişkenleri
    int t = 0;            // Simülasyon zamanı (saniye)
    int done = 0;         // Tamamlanan + timeout olan görev sayısı
    Task* running = NULL; // Şu anda CPU'da çalışan görev
    
    // Tüm görevler tamamlanana veya maksimum süreye ulaşana kadar döngü
    while (done < n && t < 2000) {
        /* 1) Arrivals - Bu zaman diliminde gelen görevleri kuyruklara ekle */
        for (int i = 0; i < n; i++) {   // Her görev için arrival kontrolü
            if (all[i]->arrival_time == t) {
                Task* x = all[i];      // Yeni gelen görev
                x->state = TASK_READY; // Sisteme girdi, artık READY
                
                // Önceliğine göre uygun kuyruğa ekle
                if (x->priority == 0) queue_enqueue(rt, x);
                else if (x->priority == 1) queue_enqueue(q1, x);
                else if (x->priority == 2) queue_enqueue(q2, x);
                else queue_enqueue(q3, x);
            }
        }
        
        /* 2) RT çalışıyorsa kesme (bitene kadar devam) */
        // Real-time görev çalışmıyorsa, öncelik sırasına göre yeni görev seç
        // Çalışan görev RT ise ve RUNNING durumundaysa onu kesmeyiz
        if (!(running && running->priority == 0 && running->state == TASK_RUNNING)) {
            running = NULL;
            
            // Öncelik sırasına göre kuyruklardan görev seç (RT > q1 > q2 > q3)
            // Önce RT kuyruğundan uygun bir görev seç
            Task* pick = dequeue_valid(rt);
            if (pick) running = pick; // RT bulundu ise onu çalıştır
            else {
                pick = dequeue_valid(q1);
                if (pick) running = pick;
                else {
                    pick = dequeue_valid(q2);
                    if (pick) running = pick;
                    else {
                        pick = dequeue_valid(q3);
                        if (pick) running = pick;
                    }
                }
            }
        }
        
        /*
         * LOG SIRASI KURALI:
         * - Devam eden RT varsa: önce yurutuluyor, sonra zamanasimi
         * - Aksi halde: önce zamanasimi, sonra basladi/yurutuluyor
         */
        // Bu adımda RT görevi bir önceki slice'tan kesintisiz devam ediyor mu?
        int continuing_rt = (running && running->priority == 0 && running->state == TASK_RUNNING);
        
        if (continuing_rt) {
            /* 3A) Önce yurutuluyor + 1 sn çalışma */
            print_log((double)t, "yurutuluyor", running); // RT görev CPU'da çalışıyor
            running->remaining_time--;                    // 1 sn çalıştı, kalan süreden düş
            if (running->start_time == -1) running->start_time = t; // İlk kez çalıştıysa başlangıç zamanını kaydet
            running->last_run_time = t + 1;               // Bu slice'ın bitiş zamanı
            
            /* 3B) Sonra timeoutlar */
            check_timeouts(t, all, n, running, &done);    // Diğer görevler için timeout kontrolü
        } else {
            /* 3A) Önce timeoutlar */
            check_timeouts(t, all, n, running, &done);    // Önce sistemde bekleyenler için timeout
            
            /* 3B) Sonra CPU logu + 1 sn çalışma */
            if (running) {
                // READY/SUSPENDED durumundan RUNNING'e ilk geçiş
                if (running->state == TASK_READY || running->state == TASK_SUSPENDED) {
                    running->state = TASK_RUNNING;
                    print_log((double)t, "basladi", running);
                } else {
                    print_log((double)t, "yurutuluyor", running);
                }
                
                running->remaining_time--;                // 1 sn CPU kullandı
                if (running->start_time == -1) running->start_time = t; // İlk kez çalıştıysa başlangıç anı
                running->last_run_time = t + 1;           // Son çalıştığı an
            }
        }
        
        /* 4) Advance time */
        t++; // Simülasyon zamanını bir saniye ilerlet
        
        /* 5) Slice end: sonlandi / askida */
        if (running) {
            // Görev süresini bitirdiyse tamamlandı olarak işaretle
            if (running->remaining_time <= 0) {
                running->state = TASK_COMPLETED;
                print_log((double)t, "sonlandi", running);
                done++; // Tamamlanan görev sayısını artır
                running = NULL;
            } else {
                // Real-time görevler kesintisiz devam eder
                if (running->priority == 0) { // RT görevler kesintiye uğramaz, aynı öncelikte devam
                    /* RT devam */
                } else {
                    // User task: önceliği düşür ve askıya al
                    running->state = TASK_SUSPENDED; // Zaman dilimi bitti, askıya al
                    running->current_priority++;     // MLFQ: Önceliği bir seviye düşür
                    
                    print_log((double)t, "askida", running);
                    
                    // Yeni önceliğine göre kuyruğa geri ekle
                    if (running->current_priority == 2) queue_enqueue(q2, running);
                    else queue_enqueue(q3, running);
                    
                    running = NULL;
                }
            }
        }
    }
    
    // FreeRTOS bellek yönetimi ile kuyrukları ve görev listesini temizle
    queue_destroy(rt);
    queue_destroy(q1);
    queue_destroy(q2);
    queue_destroy(q3);
    free_task_list(all, n);
    
    // FreeRTOS semaphore'u temizle
    if (g_print_mutex) {
        vSemaphoreDelete(g_print_mutex);
    }
}
