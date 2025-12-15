// Gerekli kütüphaneleri dahil et
#include "scheduler.h"
#include "tasks.h"
#include <stdio.h>
#include <string.h>

/* Son yazdırılan rengi ve ID'yi takip etmek için statik değişkenler */
/* sadece 1 kere tanimla */
static const char* last_printed_color = NULL;
static int last_printed_id = -1;

/* Queue'dan TIMEOUT/COMPLETED olmayan ilk elemani cek */
/* Kuyruktan geçerli (çalıştırılabilir) bir görev çıkarır */
static Task* dequeue_valid(Queue* q) {
    // Kuyruk boşalana kadar döngü
    while (!queue_is_empty(q)) {
        Task* t = queue_dequeue(q);
        if (!t) return NULL;

        // Sadece timeout veya tamamlanmamış görevleri döndür
        if (t->state != TASK_TIMEOUT && t->state != TASK_COMPLETED) {
            return t;
        }
    }
    return NULL;
}

/* Normal loglar */
/* Görev durumunu renkli olarak ekrana yazdırır */
static void print_log(double time, const char* status, Task* t) {
    // Görevin rengini al
    const char* color_to_use = t->color;

    /* ID 0016 her zaman beyaz olsun */
    if (t->id == 16) color_to_use = COLOR_WHITE;

    /* Zamanasimi normal print_log ile basilmamali ama garanti olsun */
    // Zaman aşımı durumunda kırmızı renk kullan
    if (strcmp(status, "zamanasimi") == 0) {
        color_to_use = COLOR_RED;
    } else {
        /* Farkli ID arka arkaya gelirse ayni renk olmasin */
        // Ardışık farklı görevler aynı renkte olmasın
        if (last_printed_color != NULL && last_printed_id != t->id) {
            if (color_to_use == last_printed_color) {
                /* alternatif renge gec */
                // Alternatif renge geç
                color_to_use = COLOR_WHITE;
                if (color_to_use == last_printed_color) {
                    color_to_use = COLOR_CYAN;
                }
                /* ID16 sabiti bozulmasin */
                if (t->id == 16) color_to_use = COLOR_WHITE;
            }
        }
    }

    // Görev bilgilerini formatlanmış şekilde yazdır
    printf("%s%.4f sn\tproses %-15s\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s\n",
           color_to_use,
           time,
           status,
           t->id,
           t->current_priority,
           t->remaining_time,
           COLOR_RESET);

    // Son yazdırılan rengi ve ID'yi güncelle
    last_printed_color = color_to_use;
    last_printed_id = t->id;
}

/* Zamanasimi logu: HER ZAMAN kirmizi */
/* Zaman aşımı durumunu kırmızı renkte yazdırır */
static void print_timeout(double time, Task* t) {
    // Zaman aşımı mesajını formatla
    char buf[24];
    snprintf(buf, sizeof(buf), "%-15s", "zamanasimi");

    // Zaman aşımı bilgisini kırmızı renkte yazdır
    printf(COLOR_RED "%.4f sn\tproses %s\t(id:%04d\toncelik:%d\tkalan sure:%d sn)" COLOR_RESET "\n",
           time,
           buf,
           t->id,
           t->current_priority,
           t->remaining_time);

    /* timeout satiri son yazilan renk/id olarak sayilsin ki ard-arda renk kuralin bozulmasin */
    // Son yazdırılan rengi güncelle (renk kurallarını korumak için)
    last_printed_color = COLOR_RED;
    last_printed_id = t->id;
}

/*
 * Timeout:
 * - only tasks that already arrived
 * - only tasks that are waiting (READY or SUSPENDED)
 * - not the running task
 * - reference time:
 *      if never ran: arrival_time
 *      if ran before: last_run_time
 * - (now - reference) >= 20  => timeout
 */
/* Tüm görevleri kontrol ederek zaman aşımına uğrayanları işaretler */
static void check_timeouts(int now, Task** all, int n, Task* running, int* doneCounter) {
    // Tüm görevleri dolaş
    for (int i = 0; i < n; i++) {
        Task* x = all[i];

        // Zaten tamamlanmış veya timeout olmuş görevleri atla
        if (x->state == TASK_COMPLETED || x->state == TASK_TIMEOUT) continue;
        // Şu anda çalışan görevi atla
        if (x == running) continue;
        // Henüz gelmemiş görevleri atla
        if (x->arrival_time > now) continue;

        /* sadece bekleyenler timeout olur */
        // Sadece hazır veya askıya alınmış görevler timeout olabilir
        if (!(x->state == TASK_READY || x->state == TASK_SUSPENDED)) continue;

        // Referans zamanı belirle: hiç çalışmadıysa geliş zamanı, çalıştıysa son çalışma zamanı
        int ref_time = (x->start_time == -1) ? x->arrival_time : x->last_run_time;

        // 20 saniyeden fazla beklediyse timeout
        if ((now - ref_time) >= 20) {
            x->state = TASK_TIMEOUT;
            print_timeout((double)now, x);
            (*doneCounter)++;
        }
    }
}

/* Ana simülasyon fonksiyonu - çok seviyeli geri beslemeli zaman paylaşımlı zamanlayıcıyı çalıştırır */
void run_simulation(const char* filename) {
    // Girdi dosyasından görevleri oku
    Task** all = NULL;
    int n = parse_input_file(filename, &all);
    if (n <= 0) return;

    // Öncelik seviyelerine göre kuyruklar oluştur
    Queue* rt = queue_create();  /* prio 0 - Real-time (en yüksek öncelik) */
    Queue* q1 = queue_create();  /* prio 1 - Yüksek öncelik */
    Queue* q2 = queue_create();  /* prio 2 - Orta öncelik */
    Queue* q3 = queue_create();  /* prio 3+ (RR) - Düşük öncelik, Round-Robin */

    // Simülasyon değişkenleri
    int t = 0;              // Mevcut zaman
    int done = 0;           // Tamamlanan görev sayısı
    Task* running = NULL;   // Şu anda çalışan görev

    // Tüm görevler tamamlanana veya maksimum süreye ulaşana kadar döngü
    while (done < n && t < 2000) {

        /* 1) arrivals */
        // Bu zaman diliminde gelen görevleri ilgili kuyruklara ekle
        for (int i = 0; i < n; i++) {
            if (all[i]->arrival_time == t) {
                Task* x = all[i];
                x->state = TASK_READY;

                // Önceliğine göre uygun kuyruğa ekle
                if (x->priority == 0) queue_enqueue(rt, x);
                else if (x->priority == 1) queue_enqueue(q1, x);
                else if (x->priority == 2) queue_enqueue(q2, x);
                else queue_enqueue(q3, x);
            }
        }

        /* 2) RT calisiyorsa kesme (bitene kadar devam) */
        // Real-time görev çalışmıyorsa, öncelik sırasına göre yeni görev seç
        if (!(running && running->priority == 0 && running->state == TASK_RUNNING)) {
            running = NULL;

            // Öncelik sırasına göre kuyruklardan görev seç (RT > q1 > q2 > q3)
            Task* pick = dequeue_valid(rt);
            if (pick) running = pick;
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
         * - Devam eden RT varsa: once yurutuluyor, sonra zamanasimi
         * - Aksi halde (yeni baslama / user task / bos CPU): once zamanasimi, sonra basladi/yurutuluyor
         */
        // Real-time görev devam ediyor mu kontrol et
        int continuing_rt = (running && running->priority == 0 && running->state == TASK_RUNNING);

        if (continuing_rt) {
            /* 3A) once yurutuluyor + 1 sn calisma */
            // RT görev devam ediyor: önce çalışma logu, sonra timeout kontrolü
            print_log((double)t, "yurutuluyor", running);

            // Kalan süreyi azalt
            running->remaining_time--;

            // İlk çalışma zamanını ve son çalışma zamanını güncelle
            if (running->start_time == -1) running->start_time = t;
            running->last_run_time = t + 1;

            /* 3B) sonra timeoutlar */
            check_timeouts(t, all, n, running, &done);
        } else {
            /* 3A) once timeoutlar */
            // Yeni görev veya user task: önce timeout kontrolü, sonra çalışma logu
            check_timeouts(t, all, n, running, &done);

            /* 3B) sonra CPU logu + 1 sn calisma */
            if (running) {
                // Görev yeni başlıyorsa "basladi", devam ediyorsa "yurutuluyor" yazdır
                if (running->state == TASK_READY || running->state == TASK_SUSPENDED) {
                    running->state = TASK_RUNNING;
                    print_log((double)t, "basladi", running);
                } else {
                    print_log((double)t, "yurutuluyor", running);
                }

                // Kalan süreyi azalt
                running->remaining_time--;

                // Zaman bilgilerini güncelle
                if (running->start_time == -1) running->start_time = t;
                running->last_run_time = t + 1;
            }
        }

        /* 4) advance time */
        // Zamanı bir saniye ilerlet
        t++;

        /* 5) slice end: sonlandi / askida */
        // Zaman dilimi sonu: görev tamamlandı mı yoksa askıya mı alınacak?
        if (running) {
            // Görev tamamlandı mı?
            if (running->remaining_time <= 0) {
                running->state = TASK_COMPLETED;
                print_log((double)t, "sonlandi", running);
                done++;
                running = NULL;
            } else {
                // Real-time görevler kesintisiz devam eder
                if (running->priority == 0) {
                    /* RT devam */
                } else {
                    // User task: önceliği düşür ve askıya al
                    running->state = TASK_SUSPENDED;
                    running->current_priority++;

                    print_log((double)t, "askida", running);

                    /* re-enqueue based on new priority */
                    // Yeni önceliğine göre kuyruğa geri ekle
                    if (running->current_priority == 2) queue_enqueue(q2, running);
                    else queue_enqueue(q3, running);

                    running = NULL;
                }
            }
        }
    }

    // Kuyrukları ve görev listesini temizle
    queue_destroy(rt);
    queue_destroy(q1);
    queue_destroy(q2);
    queue_destroy(q3);
    free_task_list(all, n);
}
