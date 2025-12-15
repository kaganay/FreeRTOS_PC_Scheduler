# Git Kullanım Rehberi - Hızlı Başlangıç

Bu dosya, projeyi GitHub'a yüklemek ve arkadaşlarınızla paylaşmak için adım adım rehber içerir.

## 🚀 Hızlı Başlangıç (3 Adım)

### 1️⃣ GitHub'da Repository Oluştur

1. https://github.com adresine git
2. Sağ üstteki **"+"** → **"New repository"**
3. Repository adı: `FreeRTOS_PC_Scheduler` (veya istediğiniz isim)
4. Public veya Private seç
5. **ÖNEMLİ:** "Initialize with README" ve "Add .gitignore" seçeneklerini **İŞARETLEME**
6. **"Create repository"** tıkla
7. Yeşil **"Code"** butonuna tıkla ve URL'i kopyala (örn: `https://github.com/kullaniciadi/FreeRTOS_PC_Scheduler.git`)

### 2️⃣ Terminal'de Komutları Çalıştır

PowerShell veya Terminal'i açın ve proje klasörüne gidin:

```powershell
cd "C:\Masaüstü\İsletimSistemleri\FreeRTOS_PC_Scheduler"
```

Sonra şu komutları sırayla çalıştırın (URL'yi kendi repository URL'inizle değiştirin):

```bash
# Remote repository'yi ekle
git remote add origin https://github.com/KULLANICI_ADINIZ/REPOSITORY_ADI.git

# Değişiklikleri kontrol et
git status

# Tüm dosyaları ekle (zaten eklenmiş olabilir)
git add .

# Commit et (eğer yeni değişiklik varsa)
git commit -m "Proje GitHub'a yüklendi"

# GitHub'a gönder
git push -u origin master
```

### 3️⃣ GitHub Kimlik Doğrulama

İlk kez gönderirken GitHub kullanıcı adı ve şifre isteyecek:

- **Kullanıcı adı:** GitHub kullanıcı adınızı girin
- **Şifre:** Normal şifre yerine **Personal Access Token** kullanmanız gerekir

#### Personal Access Token Oluşturma:

1. GitHub → Sağ üstte profil fotoğrafı → **Settings**
2. Sol menüden **Developer settings**
3. **Personal access tokens** → **Tokens (classic)**
4. **Generate new token** → **Generate new token (classic)**
5. Token adı verin (örn: "FreeRTOS Project")
6. Süre seçin (örn: 90 days veya No expiration)
7. **"repo"** seçeneğini işaretleyin (tüm repo yetkileri için)
8. En alta scroll edip **"Generate token"** tıklayın
9. **Token'ı kopyalayın** (bir daha gösterilmeyecek!)

**Not:** Şifre yerine bu token'ı kullanın.

---

## 📥 Arkadaşlarınız Projeyi Nasıl İndirir?

Arkadaşlarınız şu komutla projeyi indirebilir:

```bash
git clone https://github.com/KULLANICI_ADINIZ/REPOSITORY_ADI.git
cd FreeRTOS_PC_Scheduler
```

---

## 🔄 Sonraki Değişiklikleri Gönderme

Projede değişiklik yaptıktan sonra:

```bash
# Değişiklikleri görüntüle
git status

# Değişiklikleri ekle
git add .

# Commit et
git commit -m "Değişiklik açıklaması"

# GitHub'a gönder
git push
```

---

## ❓ Sık Karşılaşılan Sorunlar

### "remote origin already exists" hatası
```bash
# Mevcut remote'u kontrol et
git remote -v

# Remote'u değiştir
git remote set-url origin https://github.com/YENI_URL.git
```

### "Authentication failed" hatası
- Personal Access Token kullandığınızdan emin olun
- Token'ın "repo" yetkisine sahip olduğundan emin olun

### "branch 'master' has no upstream branch" hatası
```bash
git push -u origin master
```

### "fatal: not a git repository" hatası
```bash
# Proje klasöründe olduğunuzdan emin olun
cd FreeRTOS_PC_Scheduler
git status
```
---

## 📚 Daha Fazla Bilgi

Detaylı bilgi için `README.md` dosyasındaki "Git ile Projeyi İndirme ve Paylaşma" bölümüne bakın.