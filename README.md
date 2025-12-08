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

## Git ile Projeyi İndirme ve Paylaşma

Bu proje Git ile versiyon kontrolü altındadır. 

### Projeyi İndirme

Projeyi klonlamak için:

```bash
git clone <repository-url>
cd FreeRTOS_PC_Scheduler
```

Eğer proje zaten bir Git repository'si ise ve güncellemeleri almak istiyorsanız:

```bash
git pull
```

### Projeyi Paylaşma (Detaylı Rehber)

Projeyi arkadaşlarınızla paylaşmak için aşağıdaki adımları takip edin:

#### Adım 1: GitHub'da Repository Oluşturma

1. **GitHub'a giriş yapın:**
   - https://github.com adresine gidin
   - Hesabınız yoksa "Sign up" ile yeni hesap oluşturun

2. **Yeni repository oluşturun:**
   - Sağ üst köşedeki **"+"** butonuna tıklayın
   - **"New repository"** seçeneğini seçin
   - Repository adını girin (örn: `FreeRTOS_PC_Scheduler`)
   - **"Public"** veya **"Private"** seçin (Public = herkes görebilir, Private = sadece siz ve eklediğiniz kişiler)
   - **"Initialize this repository with a README"** seçeneğini işaretlemeyin (zaten README'miz var)
   - **"Add .gitignore"** seçeneğini de işaretlemeyin (zaten .gitignore'muz var)
   - **"Create repository"** butonuna tıklayın

3. **Repository URL'ini kopyalayın:**
   - Oluşturduğunuz repository sayfasında yeşil **"Code"** butonuna tıklayın
   - HTTPS veya SSH URL'ini kopyalayın (HTTPS daha kolay, örnek: `https://github.com/kullaniciadi/FreeRTOS_PC_Scheduler.git`)

#### Adım 2: Remote Repository'yi Eklemek

Terminal/PowerShell'de proje klasöründe şu komutu çalıştırın (URL'yi kendi repository URL'inizle değiştirin):

```bash
git remote add origin https://github.com/kullaniciadi/FreeRTOS_PC_Scheduler.git
```

**Not:** Eğer remote zaten eklenmişse hata alırsınız. Kontrol etmek için:
```bash
git remote -v
```

Eğer remote varsa ve değiştirmek istiyorsanız:
```bash
git remote set-url origin https://github.com/kullaniciadi/FreeRTOS_PC_Scheduler.git
```

#### Adım 3: Değişiklikleri Göndermek

1. **Tüm değişiklikleri ekleyin:**
   ```bash
   git add .
   ```

2. **Commit edin (eğer yeni değişiklik varsa):**
   ```bash
   git commit -m "Proje hazır"
   ```

3. **GitHub'a gönderin:**
   ```bash
   git push -u origin master
   ```
   
   **İlk kez gönderirken GitHub kullanıcı adı ve şifre/token isteyebilir:**
   - Kullanıcı adınızı girin
   - Şifre yerine **Personal Access Token** kullanmanız gerekebilir (GitHub güvenlik politikası)
   - Token oluşturmak için: GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic) → Generate new token

#### Adım 4: Arkadaşlarınızın Projeyi İndirmesi

Arkadaşlarınız projeyi şu şekilde indirebilir:

```bash
git clone https://github.com/kullaniciadi/FreeRTOS_PC_Scheduler.git
cd FreeRTOS_PC_Scheduler
```

#### Alternatif: GitLab veya Bitbucket Kullanımı

**GitLab için:**
1. https://gitlab.com adresine gidin ve repository oluşturun
2. URL formatı: `https://gitlab.com/kullaniciadi/FreeRTOS_PC_Scheduler.git`

**Bitbucket için:**
1. https://bitbucket.org adresine gidin ve repository oluşturun
2. URL formatı: `https://bitbucket.org/kullaniciadi/FreeRTOS_PC_Scheduler.git`

Komutlar aynı şekilde çalışır, sadece URL değişir.

### Git Temel Komutlar

- `git status` - Değişiklikleri görüntüle
- `git add .` - Tüm değişiklikleri stage'e ekle
- `git commit -m "mesaj"` - Değişiklikleri commit et
- `git push` - Değişiklikleri remote repository'ye gönder
- `git pull` - Remote repository'den güncellemeleri çek

## Gereksinimler

- GCC derleyici
- Make
- POSIX uyumlu işletim sistemi (Linux, macOS, WSL)
- Git (projeyi indirmek ve paylaşmak için)

## Derleme (WSL / Linux)

> Not: Proje FreeRTOS POSIX portunu kullanır. Windows/MinGW altında derleme hataları oluşur. WSL/Ubuntu veya bir Linux ortamında derleyin.

```bash
cd FreeRTOS_PC_Scheduler
make clean
make
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

