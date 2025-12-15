// Header guard: dosyanın birden fazla kez dahil edilmesini önler
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>

/* Renkler */
// ANSI renk kodları - terminal çıktısını renklendirmek için
#define COLOR_RESET   "\033[0m"   // Renk sıfırlama
#define COLOR_RED     "\033[31m"  // Kırmızı
#define COLOR_GREEN   "\033[32m"  // Yeşil
#define COLOR_YELLOW  "\033[33m"  // Sarı
#define COLOR_BLUE    "\033[34m"  // Mavi
#define COLOR_MAGENTA "\033[35m"  // Magenta
#define COLOR_CYAN    "\033[36m"  // Cyan
#define COLOR_WHITE   "\033[37m"  // Beyaz

/* Durumlar */
// Görev durumları: görevin sistem içindeki mevcut durumunu temsil eder
typedef enum {
    TASK_READY,      // Hazır - çalıştırılmayı bekliyor
    TASK_RUNNING,    // Çalışıyor - şu anda CPU'da
    TASK_SUSPENDED,  // Askıda - zaman dilimi bitti, bekliyor
    TASK_COMPLETED,  // Tamamlandı - görev başarıyla bitti
    TASK_TIMEOUT     // Zaman aşımı - 20 saniye içinde çalıştırılamadı
} TaskState;

// Görev yapısı: her görevin tüm bilgilerini tutar
typedef struct Task {
    int id;                  // Görev kimlik numarası
    int priority;           /* giris onceligi (0..3) - başlangıç önceliği */
    int current_priority;   /* calisirken dusurulen oncelik - geri besleme ile değişen öncelik */
    int arrival_time;       /* sn - görevin sisteme geldiği zaman */
    int burst_time;         /* sn - görevin toplam çalışma süresi */
    int remaining_time;     /* sn - görevin kalan çalışma süresi */
    TaskState state;         // Görevin mevcut durumu

    /* timeout icin gerekli */
    int start_time;         /* ilk kez CPU aldigi zaman, yoksa -1 */
    int last_run_time;      /* en son CPU kullanimini BITIRDIGI zaman, yoksa -1 */

    const char* color;       // Terminal çıktısı için renk
    struct Task* next;       // Kuyruk için bağlı liste pointer'ı
} Task;

/* Kuyruk */
// FIFO (First In First Out) kuyruk yapısı - bağlı liste ile implementasyon
typedef struct {
    Task* front;  // Kuyruğun başı - ilk çıkacak eleman
    Task* rear;   // Kuyruğun sonu - son eklenen eleman
    int size;     // Kuyruktaki eleman sayısı
} Queue;

/* Scheduler ana fonksiyonu */
// Girdi dosyasını okuyup simülasyonu çalıştırır
void run_simulation(const char* filename);

#endif
