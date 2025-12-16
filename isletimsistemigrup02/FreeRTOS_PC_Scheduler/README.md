# FreeRTOS PC Scheduler

Bu proje, FreeRTOS'un görev (task) sıralayıcısının nasıl çalıştığını PC üzerinde POSIX (Linux/Windows) ortamında simüle eder. 4 seviyeli öncelikli scheduler implementasyonu içerir:

- **Öncelik 0**: Gerçek zamanlı görevler (FCFS algoritması)
- **Öncelik 1-3**: Kullanıcı görevleri (Multi-level Feedback Queue - MLFQ)

Proje gereksinimlerine göre kendi scheduler fonksiyonlarını kullanır ve FreeRTOS çekirdeğini tanımayı hedefler.

## Hızlı Başlangıç

### Windows

```cmd
cd FreeRTOS_PC_Scheduler-master
make clean
make
freertos_sim giris.txt
```

### Linux/WSL

```bash
cd FreeRTOS_PC_Scheduler-master
make clean
make
./freertos_sim giris.txt
```

## Proje Yapısı

```
FreeRTOS_PC_Scheduler/
├── src/                        # Kaynak kod dosyaları
│   ├── main.c                  # Ana program giriş noktası
│   ├── scheduler.c             # Scheduler implementasyonu (4 seviyeli öncelikli)
│   ├── scheduler.h             # Scheduler header dosyası
│   ├── tasks.c                 # Görev fonksiyonları ve kuyruk işlemleri
│   ├── tasks.h                 # Tasks header dosyası
│   └── freertos_hooks.c        # FreeRTOS hook fonksiyonları
├── FreeRTOS/                   # FreeRTOS kaynak kodu
│   ├── FreeRTOSConfig.h        # FreeRTOS yapılandırması
│   ├── include/                # FreeRTOS header dosyaları
│   ├── source/                 # FreeRTOS kaynak dosyaları
│   └── portable/               # Platform-specific port dosyaları
├── Makefile                    # Derleme dosyası (Windows/Linux)
├── giris.txt                   # Görev listesi (CSV format)
└── README.md                   # Bu dosya
```

## Gereksinimler

- **Windows:** MinGW (GCC + make)

  - **Derleyici:** `gcc` (proje gereksinimlerine göre)
  - **Make:** `make` komutunun çalışması için:

    **1. make.exe oluşturun (bir kez):**

    ```powershell
    cd C:\MinGW\bin
    copy mingw32-make.exe make.exe
    ```

    **2. PATH'e ekleyin (kalıcı olarak):**

    ```powershell
    # PowerShell'i yönetici olarak açın ve çalıştırın:
    [Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\MinGW\bin", "User")
    ```

    Veya sistem özelliklerinden PATH değişkenine `C:\MinGW\bin` ekleyin.

  - **Not:** Standart MinGW'de pthread desteği sınırlı olabilir. Proje FreeRTOS POSIX portu kullanır.

- **Linux/WSL:** GCC + Make + pthread

## Kullanım

### Windows

**1. Proje klasörüne girin:**

```cmd
cd FreeRTOS_PC_Scheduler-master
```

**2. İlk Kurulum (Sadece bir kez):**

MinGW bin dizininde `make.exe` oluşturun:

```cmd
cd C:\MinGW\bin
copy mingw32-make.exe make.exe
```

**3. Derleme:**

```cmd
cd FreeRTOS_PC_Scheduler-master
make clean
make
```

**4. Çalıştırma:**

```cmd
freertos_sim.exe giris.txt
```

### Linux/WSL

```bash
make clean
make
```

### Linux/WSL

**1. Proje klasörüne girin:**

```bash
cd FreeRTOS_PC_Scheduler-master
```

**2. Derleme:**

```bash
make clean
make
```

**3. Çalıştırma:**

```bash
./freertos_sim giris.txt
```

veya Makefile ile:

```bash
make test
```

## Giriş Dosyası Formatı

`giris.txt` dosyası CSV formatında görev tanımları içerir:

```
<varış_zamanı>,<öncelik>,<görev_süresi>
```

Örnek:

```
12,0,1    # Zaman 12'de varış, öncelik 0 (RT), 1 saniye
12,1,2    # Zaman 12'de varış, öncelik 1, 2 saniye
13,3,6    # Zaman 13'te varış, öncelik 3, 6 saniye
```

### Öncelik Seviyeleri

- **0**: Gerçek zamanlı (Real-Time) - En yüksek öncelik, FCFS
- **1**: Yüksek öncelikli kullanıcı görevi
- **2**: Orta öncelikli kullanıcı görevi
- **3**: Düşük öncelikli kullanıcı görevi

## Scheduler Algoritması

Bu proje **gerçek FreeRTOS kütüphanesini** kullanarak çalışır:

1. **Gerçek Zamanlı Görevler (Öncelik 0)**: FreeRTOS'un en yüksek öncelik seviyesinde çalışır, tamamlanana kadar kesintisiz çalışır
2. **Kullanıcı Görevleri (Öncelik 1-3)**: Multi-level feedback queue (MLFQ) FreeRTOS task priority sistemi ile implement edilir
   - Zaman kuantumu: 1 saniye
   - Her kuantum sonunda görev, bir alt öncelik seviyesine düşer (FreeRTOS priority değiştirilir)
   - En alt seviyede (3) Round Robin uygulanır

## Özellikler

- Renkli terminal çıktısı (her görev için farklı renk)
- Zaman damgalı mesajlar
- Görev durumu takibi (başlangıç, çalışma, askıya alma, tamamlanma)
- Preemption (yüksek öncelikli görevler düşük önceliklileri keser)

## Makefile Komutları

```cmd
make          # Projeyi derle
make clean    # Derleme dosyalarını temizle
make test     # Programı test et (giris.txt ile)
make help     # Yardım mesajını göster
```

## Notlar

- Proje **gerçek FreeRTOS kütüphanesini** kullanır (simülasyon değil)
- FreeRTOS Posix portu kullanılır (pthread tabanlı)
- Windows'ta MinGW ile pthread desteği gereklidir
- Simülasyon maksimum 2000 saniye çalışır
- Görevler maksimum 20 saniye içinde başlatılmazsa timeout olur
- Windows terminalinde ANSI renk kodları çalışmayabilir (WSL'de düzgün çalışır)
- FreeRTOS tick rate: 1000 Hz (1 tick = 1ms)
