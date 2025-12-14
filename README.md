# FreeRTOS PC Scheduler Simülasyonu

Bu proje, FreeRTOS'un görev sıralayıcısını PC üzerinde simüle eder. Windows ve Linux ortamlarında çalışır.

## Proje Yapısı

```
FreeRTOS_PC_Scheduler/
├── src/                        # Kaynak kod dosyaları
│   ├── main.c                  # Ana program
│   ├── scheduler.c             # Scheduler implementasyonu
│   ├── scheduler.h             # Scheduler header
│   ├── tasks.c                 # Görev fonksiyonları
│   └── freertos_hooks.c        # FreeRTOS hook fonksiyonları
├── Makefile                    # Derleme dosyası (Windows/Linux)
├── giris.txt                   # Görev listesi (CSV format)
└── README.md                   # Bu dosya
```

## Gereksinimler

- **Windows:** MinGW (GCC + mingw32-make)
- **Linux/WSL:** GCC + Make

## Derleme

### Windows (MinGW)

```cmd
mingw32-make clean
mingw32-make
```

### Linux/WSL

```bash
make clean
make
```

## Çalıştırma

### Windows

```cmd
freertos_sim giris.txt
```

veya Makefile ile:

```cmd
mingw32-make test
```

### Linux/WSL

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

1. **Gerçek Zamanlı Görevler (Öncelik 0)**: FCFS algoritması ile tamamlanana kadar kesintisiz çalışır
2. **Kullanıcı Görevleri (Öncelik 1-3)**: Multi-level feedback queue (MLFQ)
   - Zaman kuantumu: 1 saniye
   - Her kuantum sonunda görev, bir alt öncelik kuyruğuna düşer (min seviye: 3)
   - En alt seviyede (3) Round Robin uygulanır; kuantum sonrası görev kuyruğun sonuna eklenir

## Özellikler

- Renkli terminal çıktısı (her görev için farklı renk)
- Zaman damgalı mesajlar
- Görev durumu takibi (başlangıç, çalışma, askıya alma, tamamlanma)
- Preemption (yüksek öncelikli görevler düşük önceliklileri keser)

## Makefile Komutları

```cmd
mingw32-make          # Projeyi derle
mingw32-make clean    # Derleme dosyalarını temizle
mingw32-make test     # Programı test et (giris.txt ile)
mingw32-make help     # Yardım mesajını göster
```

## Notlar

- Proje Windows'ta MinGW ile test edilmiştir
- Simülasyon maksimum 1000 saniye çalışır
- Görevler maksimum 20 saniye çalışabilir
- Windows terminalinde ANSI renk kodları çalışmayabilir (WSL'de düzgün çalışır)
