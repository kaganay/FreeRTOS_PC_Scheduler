# FreeRTOS Kullanarak Basit Bir Görev Sıralayıcısı (Scheduler) Simülasyonu
## Teknik Tasarım Raporu

---

## 1. Giriş

FreeRTOS, gömülü sistemler için tasarlanmış açık kaynaklı bir gerçek zamanlı işletim sistemi (RTOS) çekirdeğidir. Gerçek zamanlı işletim sistemleri, zamanlama garantileri sağlayarak kritik görevlerin belirli zaman aralıklarında tamamlanmasını garanti eder. Bu sistemler, özellikle otomotiv, havacılık, endüstriyel kontrol ve IoT cihazları gibi zaman kritik uygulamalarda yaygın olarak kullanılmaktadır.

Bu proje, FreeRTOS'un görev sıralayıcısının (scheduler) çalışma mantığını PC üzerinde POSIX ortamında simüle etmek amacıyla geliştirilmiştir. Proje, dört seviyeli öncelikli bir görev sıralayıcısı implementasyonu içermektedir: gerçek zamanlı görevler için FCFS (First-Come-First-Served) algoritması ve kullanıcı görevleri için üç seviyeli geri beslemeli kuyruk (Multi-Level Feedback Queue - MLFQ) mekanizması.

---

## 2. Amaç ve Yöntem

### 2.1 Proje Amacı

Bu projenin temel amacı, FreeRTOS'un görev zamanlama mekanizmasını anlamak ve PC ortamında simüle etmektir. Proje kapsamında:

- FreeRTOS çekirdeğinin görev yönetimi yapısını öğrenmek
- Öncelik temelli görev zamanlamanın nasıl çalıştığını gözlemlemek
- Gerçek zamanlı görevlerin kullanıcı görevlerinden nasıl ayrıldığını anlamak
- Multi-level feedback queue (MLFQ) algoritmasının implementasyonunu gerçekleştirmek
- Görevlerin bağlam değişimlerini (context switching) ve zaman dilimlerinin (time quantum) etkileşimini simüle etmek

hedeflenmektedir.

### 2.2 Scheduler Yapısının Tasarlanması

Proje, dört seviyeli öncelik sistemine sahip bir scheduler tasarımı içermektedir:

**Seviye 0 - Gerçek Zamanlı (RT) Görevler:**
- En yüksek öncelik seviyesi
- FCFS algoritması ile çalıştırılır
- Tamamlanana kadar kesintisiz yürütülür (non-preemptive)
- Düşük öncelikli görevler RT görevler tarafından kesintiye uğratılabilir (preemption)

**Seviye 1-3 - Kullanıcı Görevleri:**
- Üç seviyeli geri beslemeli kuyruk (MLFQ) mekanizması
- Zaman kuantumu: 1 saniye
- Her kuantum sonunda tamamlanmayan görevler bir alt öncelik seviyesine düşürülür
- En alt seviyede (Seviye 3) Round-Robin algoritması uygulanır

### 2.3 Yöntem

Proje, FreeRTOS'un POSIX portunu kullanarak PC ortamında çalışacak şekilde tasarlanmıştır. FreeRTOS'un `portable/ThirdParty/GCC/Posix` klasöründeki POSIX portu, FreeRTOS çekirdeğinin Linux/Unix ortamlarında çalışmasını sağlar. Bu port, pthread kütüphanesini kullanarak FreeRTOS'un thread yönetimini simüle eder.

Simülasyon, deterministik bir yaklaşımla gerçekleştirilmiştir. Her zaman adımında (tick) sadece bir görev işlenir ve görev durumları (başlangıç, çalışma, askıya alma, tamamlanma) zaman damgalı mesajlarla loglanır. Bu yaklaşım, scheduler'ın davranışını gözlemlemeyi ve analiz etmeyi kolaylaştırır.

---

## 3. Yöntem: FreeRTOS Görev Yönetimi ve Scheduler Fonksiyonları

### 3.1 FreeRTOS Entegrasyonu

Proje, FreeRTOS çekirdeğinin temel bileşenlerini içermektedir:

- **Görev Yönetimi (Task Management):** `tasks.c` modülü görev oluşturma, silme ve durum yönetimi sağlar
- **Kuyruk Yönetimi (Queue Management):** `queue.c` modülü görevler arası iletişim için kuyruk mekanizması sağlar
- **Bellek Yönetimi:** `heap_3.c` modülü standart `malloc()` ve `free()` fonksiyonlarını kullanarak dinamik bellek ayırma yapar
- **POSIX Portu:** `port.c` ve `wait_for_event.c` modülleri FreeRTOS'un POSIX ortamında çalışmasını sağlar

### 3.2 Scheduler Modülleri ve Ana İşlevler

Proje, modüler bir yapıya sahiptir ve şu ana bileşenlerden oluşur:

#### 3.2.1 Görev Yönetimi Modülü (`tasks.c`)

Bu modül, görevlerin oluşturulması, yönetimi ve görselleştirilmesinden sorumludur. Her görev için benzersiz bir renk şeması atanır ve görev durumları (hazır, çalışıyor, askıya alınmış, tamamlandı) takip edilir.

**Ana İşlevler:**
- Görev oluşturma: Giriş dosyasından okunan görev bilgilerine göre görev yapıları oluşturulur
- Görev durumu yönetimi: Görevlerin durumları (ready, running, suspended, completed) yönetilir
- Görselleştirme: Her görev için renkli terminal çıktısı sağlanır

#### 3.2.2 Kuyruk Yönetimi Modülü (`scheduler.c`)

Bu modül, scheduler'ın temel veri yapılarını ve kuyruk işlemlerini içerir.

**Veri Yapıları:**
- **Task Yapısı:** Görev kimliği, öncelik seviyesi, varış zamanı, çalışma süresi, kalan süre, mevcut öncelik seviyesi ve durum bilgilerini içerir
- **Queue Yapısı:** FIFO (First-In-First-Out) prensibiyle çalışan bağlı liste tabanlı kuyruk yapısı
- **SchedulerState Yapısı:** Scheduler'ın mevcut durumunu (zaman, çalışan görev, kuyruklar) tutar

**Ana İşlevler:**
- `queue_create()`: Yeni bir kuyruk yapısı oluşturur ve bellek ayırır
- `queue_enqueue()`: Görevi kuyruğun sonuna ekler (FIFO)
- `queue_dequeue()`: Kuyruğun başındaki görevi çıkarır ve döndürür
- `queue_is_empty()`: Kuyruğun boş olup olmadığını kontrol eder
- `parse_input_file()`: Giriş dosyasını parse eder ve görev listesi oluşturur

#### 3.2.3 Ana Simülasyon Modülü (`main.c`)

Bu modül, scheduler'ın ana döngüsünü ve görev zamanlama mantığını içerir.

**Ana İşlevler:**
- Giriş dosyası okuma ve görev listesi oluşturma
- Görevleri varış zamanına göre sıralama
- Ana simülasyon döngüsü:
  1. Yeni gelen görevleri uygun kuyruklara ekleme
  2. Öncelik sırasına göre görev seçimi (RT öncelikli)
  3. Görev yürütme (1 saniye kuantum)
  4. Görev durumu güncelleme (tamamlanma veya askıya alma)
  5. Öncelik düşürme ve yeniden kuyruğa ekleme (MLFQ)

### 3.3 Bellek ve Kaynak Yönetimi

#### 3.3.1 Bellek Ayırma Şeması

Proje, FreeRTOS'un `heap_3.c` bellek yönetim şemasını kullanmaktadır. Bu şema, standart C kütüphanesinin `malloc()` ve `free()` fonksiyonlarını kullanır. Bu yaklaşım:

- **Avantajları:**
  - Basit ve anlaşılır implementasyon
  - Standart C kütüphanesi ile uyumlu
  - PC ortamında test edilmesi kolay
  - Fragmentation sorunları işletim sistemi tarafından yönetilir

- **Dezavantajları:**
  - Deterministik olmayan bellek ayırma süreleri (gerçek zamanlı sistemler için uygun değil)
  - Bellek fragmentasyonu riski
  - Gömülü sistemlerde kullanım için uygun değil (heap_4 veya heap_5 tercih edilir)

#### 3.3.2 Kaynak Yönetimi Yapıları

**Kuyruk Yönetimi:**
- Her öncelik seviyesi için ayrı bir kuyruk yapısı (`Queue`) oluşturulur
- Kuyruklar, bağlı liste (linked list) yapısı kullanılarak implemente edilmiştir
- Kuyruk yapıları dinamik olarak `malloc()` ile ayrılır ve kullanım sonrası `free()` ile serbest bırakılır

**Görev Yönetimi:**
- Görev yapıları (`Task`) giriş dosyası okunurken dinamik olarak oluşturulur
- Görevler, bir görev dizisinde (`Task**`) tutulur ve kuyruklara referans olarak eklenir
- Görevler, simülasyon sonunda `free_task_list()` fonksiyonu ile temizlenir

**Scheduler Durumu:**
- `SchedulerState` yapısı, scheduler'ın mevcut durumunu tutar
- Bu yapı, simülasyon başlangıcında oluşturulur ve sonunda temizlenir

---

## 4. Uygulama: Çalışma Mantığı ve Çıktı Analizi

### 4.1 Scheduler Algoritması

Simülasyon, aşağıdaki adımları takip eder:

1. **Başlangıç:** Giriş dosyası okunur, görevler varış zamanına göre sıralanır ve kuyruklar oluşturulur.

2. **Her Zaman Adımında (Tick):**
   - Yeni gelen görevler (varış zamanı <= mevcut zaman) uygun kuyruklara eklenir
   - RT görevler RT kuyruğuna, kullanıcı görevleri mevcut öncelik seviyelerine göre ilgili kuyruklara eklenir

3. **Görev Seçimi:**
   - Öncelik 1: RT kuyruğu boş değilse, RT görev seçilir (FCFS)
   - Öncelik 2: RT kuyruğu boşsa, en yüksek öncelikli kullanıcı kuyruğundan görev seçilir (Seviye 1 → 2 → 3)

4. **Görev Yürütme:**
   - Seçilen görev 1 saniye (kuantum) çalıştırılır
   - Görev durumu "running" olarak işaretlenir ve log mesajı yazdırılır
   - Kalan süre 1 saniye azaltılır

5. **Görev Tamamlanma Kontrolü:**
   - Kalan süre <= 0 ise: Görev tamamlandı olarak işaretlenir ve log mesajı yazdırılır
   - Kalan süre > 0 ise:
     - RT görevler: RT kuyruğuna geri eklenir (kesintisiz devam)
     - Kullanıcı görevleri: Öncelik seviyesi düşürülür (mümkünse) ve ilgili kuyruğa eklenir, askıya alma log mesajı yazdırılır

6. **Zaman İlerletme:** Mevcut zaman 1 saniye artırılır ve döngü devam eder.

### 4.2 Çalışma Çıktısı

Simülasyon çıktısı, her görev için zaman damgalı mesajlar içerir:

- **Başlangıç Mesajı:** Görev başladığında görev kimliği, öncelik ve kalan süre bilgisi
- **Yürütme Mesajı:** Her saniye görev yürütülürken görev durumu ve kalan süre bilgisi
- **Askıya Alma Mesajı:** Görev kuantum sonunda tamamlanmadıysa askıya alınır ve öncelik düşürülür
- **Tamamlanma Mesajı:** Görev tamamlandığında son durum bilgisi

Her görev için benzersiz bir renk şeması kullanılarak çıktı görselleştirilir. Bu, görevlerin zaman çizelgesinde (timeline) kolayca takip edilmesini sağlar.

### 4.3 Test Sonuçları

Test senaryosu, 25 görev içeren bir giriş dosyası ile gerçekleştirilmiştir. Simülasyon sonuçları:

- **Toplam Süre:** 70 saniye
- **Tamamlanan Görevler:** 25/25 (100%)
- **RT Görevler:** Öncelikli olarak tamamlandı (zaman 0-30 arası)
- **Kullanıcı Görevleri:** MLFQ mekanizması ile yönetildi ve tüm seviyelerde round-robin uygulandı

Sonuçlar, scheduler'ın doğru çalıştığını ve öncelik kurallarının doğru uygulandığını göstermektedir.

---

## 5. Sonuç ve Değerlendirme

### 5.1 Görev Önceliği ve Deterministik Davranış

Proje, dört seviyeli öncelik sisteminin başarılı bir şekilde implementasyonunu göstermektedir. RT görevler, her zaman kullanıcı görevlerinden önce çalıştırılmış ve tamamlanana kadar kesintisiz yürütülmüştür. Bu, gerçek zamanlı sistemlerin temel gereksinimlerinden biri olan deterministik davranışı sağlamaktadır.

MLFQ mekanizması, kısa süreli görevlerin hızlı tamamlanmasını sağlarken, uzun süreli görevlerin adil bir şekilde kaynak kullanmasına olanak tanımaktadır. Öncelik düşürme mekanizması, görevlerin davranışlarına göre dinamik olarak öncelik seviyelerini ayarlar.

### 5.2 FreeRTOS'un Etkinliği

FreeRTOS'un POSIX portu, PC ortamında gerçek zamanlı sistem davranışını simüle etmek için etkili bir araçtır. Ancak, gerçek gömülü sistemlerde FreeRTOS'un kendi scheduler'ı kullanılır ve bu scheduler, POSIX thread'lerinden daha hafif ve deterministik bir yapıya sahiptir.

Proje, FreeRTOS'un temel bileşenlerini (görev yönetimi, kuyruk yönetimi, bellek yönetimi) kullanarak, gerçek zamanlı sistem kavramlarını öğrenmek için değerli bir deneyim sağlamaktadır.

### 5.3 Gerçek İşletim Sistemleri ile Karşılaştırma

Gerçek işletim sistemleri (Linux, Windows, macOS), benzer çok seviyeli görev zamanlama şemaları kullanır:

- **Linux:** CFS (Completely Fair Scheduler) ve RT scheduler
- **Windows:** Priority-based preemptive scheduler
- **macOS:** Grand Central Dispatch (GCD) ve priority queues

Bu sistemler, MLFQ benzeri mekanizmalar kullanarak görevlerin önceliklerini dinamik olarak ayarlar. Ancak, gerçek sistemlerde:

- **Daha Karmaşık Öncelik Hesaplama:** Görevlerin geçmiş performansı, I/O bekleme süreleri ve kullanıcı etkileşimleri dikkate alınır
- **Load Balancing:** Çok çekirdekli sistemlerde görevler farklı CPU'lara dağıtılır
- **I/O Optimizasyonu:** I/O yoğun görevler önceliklendirilir
- **Enerji Yönetimi:** Düşük güç modları için görev zamanlaması optimize edilir

### 5.4 İyileştirme Önerileri ve Eksiklikler

#### 5.4.1 Mevcut Eksiklikler

1. **Gerçek Preemption:** Mevcut implementasyon, deterministik simülasyon için tek görev/tick yaklaşımı kullanır. Gerçek FreeRTOS'ta, yüksek öncelikli görevler düşük öncelikli görevleri herhangi bir zamanda kesintiye uğratabilir.

2. **I/O Yönetimi:** Proje, I/O işlemleri ve bekleme durumlarını simüle etmez. Gerçek sistemlerde, I/O bekleyen görevler bloke edilir ve CPU'yu serbest bırakır.

3. **Çok Çekirdekli Desteği:** Mevcut implementasyon, tek çekirdekli sistemler için tasarlanmıştır. Modern sistemlerde çok çekirdekli zamanlama kritik öneme sahiptir.

4. **Bellek Fragmentasyonu:** `heap_3.c` kullanımı, uzun süreli çalışmalarda bellek fragmentasyonu riski taşır. Gömülü sistemlerde `heap_4.c` veya `heap_5.c` tercih edilmelidir.

5. **Güç Yönetimi:** Düşük güç modları ve enerji optimizasyonu mevcut değildir.

#### 5.4.2 İyileştirme Önerileri

1. **Gerçek Preemption Implementasyonu:** FreeRTOS'un gerçek preemption mekanizmasını kullanarak, yüksek öncelikli görevlerin düşük önceliklileri kesintiye uğratması sağlanabilir.

2. **I/O Simülasyonu:** Görevler için I/O bekleme durumları eklenebilir ve I/O tamamlandığında görevlerin uyandırılması simüle edilebilir.

3. **Performans Metrikleri:** Ortalama bekleme süresi, turnaround time, throughput gibi metrikler hesaplanabilir ve analiz edilebilir.

4. **Görselleştirme:** Görev zaman çizelgesi (Gantt chart) ve kuyruk durumu görselleştirmesi eklenebilir.

5. **Konfigürasyon Dosyası:** Zaman kuantumu, öncelik seviyeleri ve diğer parametreler konfigürasyon dosyasından okunabilir.

### 5.5 Bellek ve Kaynak Ayırma Şemaları

Mevcut implementasyon, basit bir bellek yönetimi şeması kullanmaktadır. Gerçek sistemlerde:

- **Statik Bellek Ayırma:** Kritik görevler için statik bellek ayırma tercih edilir (deterministik)
- **Pool Allocator:** Sık kullanılan bellek boyutları için önceden ayrılmış bellek havuzları
- **Memory Protection:** Görevler arası bellek koruması (MPU kullanımı)
- **Garbage Collection:** Bazı sistemlerde otomatik bellek yönetimi

Bu iyileştirmeler, sistemin güvenilirliğini ve performansını artırabilir.

---

## 6. Kaynaklar

- FreeRTOS Resmi Dokümantasyonu: https://www.freertos.org/
- FreeRTOS GitHub Repository: https://github.com/FreeRTOS/FreeRTOS-Kernel
- POSIX Thread Programming: IEEE Std 1003.1-2017
- Operating System Concepts (Silberschatz, Galvin, Gagne)
- Real-Time Systems Design Principles (Joseph, Pandya)

---

**Not:** Bu rapor, projenin teknik tasarımını ve implementasyon detaylarını açıklamaktadır. Kaynak kod içermemektedir ve teorik/teknik bir değerlendirme sunmaktadır.

