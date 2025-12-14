/* FreeRTOS hooks - Bu projede FreeRTOS API kullanılmadığı için boş bırakıldı */
// FreeRTOS hook fonksiyonları - assert hatası durumunda çağrılır
#include <stdio.h>
#include <stdlib.h>

/* Basit assert: mesaj yaz ve dur */
// Assert hatası durumunda çağrılan hook fonksiyonu
void vAssertCalled( const char * const pcFileName, unsigned long ulLine )
{
    // Hata mesajını yazdır (dosya adı ve satır numarası ile)
    printf("ASSERT: %s:%lu\n", pcFileName, ulLine);
    fflush(stdout);  // Çıktıyı hemen göster
    abort();         // Programı sonlandır
}
