# Projenin çalışan son hali İşletimSistemiGrup02 klasörü içerisinde bulunan projedir, diğer duran kodlar ve içerikler projenin geliştirme aşamasından kalan kodlardır.

# FreeRTOS PC Scheduler Simülasyonu

Bu proje, FreeRTOS'un görev sıralayıcısını PC üzerinde POSIX ortamında simüle eder.

## Proje Yapısı

```
FreeRTOS_PC_Scheduler/
├── FreeRTOS/                    # FreeRTOS kaynak kodu
│   ├── include/                # FreeRTOS header dosyaları
│   ├── source/                 # FreeRTOS çekirdek kaynak dosyaları
│   └── portable/               # Platform-specific port dosyaları
│       └── ThirdParty/GCC/Posix/
├── src/                        # Kaynak kod dosyaları
│   ├── main.c                  # Ana program
│   ├── scheduler.c             # Scheduler implementasyonu
│   ├── scheduler.h             # Scheduler header
│   ├── tasks.c                 # Görev fonksiyonları
│   └── freertos_hooks.c        # FreeRTOS hook fonksiyonları
├── Makefile                    # Derleme dosyası
├── build.bat                   # Windows derleme scripti
├── giris.txt                   # Görev listesi (CSV format)
└── README.md                   # Bu dosya
```

## Gereksinimler

- GCC derleyici
- Make
- POSIX uyumlu işletim sistemi (Linux, macOS, WSL)

## Derleme (WSL / Linux)

> Not: Proje FreeRTOS POSIX portunu kullanır. Windows/MinGW altında derleme hataları oluşur. WSL/Ubuntu veya bir Linux ortamında derleyin.

```bash
cd FreeRTOS_PC_Scheduler
make clean
make
```
## Çalıstırma Windows PowerShell / CMD
```bash
.\freertos_sim giris.txt

```
## Çalıştırma (WSL / Linux)

```bash
./freertos_sim giris.txt
# veya
./freertos_sim test_input.txt
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

## Temizlik

```bash
make clean
```

## Notlar

- Bu proje FreeRTOS'un POSIX portunu kullanır
- Simülasyon maksimum 1000 saniye çalışır
- Görevler maksimum 20 saniye çalışabilir

