// Ana program giriş noktası
// FreeRTOS kullanarak görev sıralayıcısı simülasyonu
// Proje gereksinimlerine göre: FreeRTOS çekirdeği kullanılır, scheduler mantığı kendi kodumuzda

// Scheduler arayüzü: run_simulation fonksiyonu burada tanımlı
#include "scheduler.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>   // printf, stderr çıktıları için

// Program başlangıç fonksiyonu
int main(int argc, char* argv[]) {
    // Komut satırı argümanlarını kontrol et
    if (argc < 2) {  // Girdi dosyası verilmemişse kullanıcıyı bilgilendir
        printf("Kullanim: %s giris.txt\n", argv[0]);
        return 1;
    }
    
    // Simülasyonu başlat (girdi dosyası adı ile)
    // scheduler.c içindeki run_simulation fonksiyonu çağrılır
    // Bu fonksiyon 4 seviyeli öncelikli scheduler'ı çalıştırır:
    // - Öncelik 0: Gerçek zamanlı görevler (FCFS algoritması)
    // - Öncelik 1-3: Kullanıcı görevleri (Multi-level Feedback Queue - MLFQ)
    // 
    // FreeRTOS çekirdeği kullanılır ama scheduler mantığı (öncelik seçimi, MLFQ)
    // kendi kodumuzda implement edilmiştir.
    run_simulation(argv[1]);
    
    return 0;
}
