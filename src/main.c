// Ana program giriş noktası
#include "scheduler.h"
#include <stdio.h>

// Program başlangıç fonksiyonu
int main(int argc, char* argv[]) {
    // Komut satırı argümanlarını kontrol et
    if (argc < 2) {
        // Girdi dosyası belirtilmemişse kullanım mesajı göster
        printf("Kullanim: %s giris.txt\n", argv[0]);
        return 1;  // Hata kodu ile çık
    }
    // Simülasyonu başlat (girdi dosyası adı ile)
    run_simulation(argv[1]);
    return 0;  // Başarılı çıkış
}
