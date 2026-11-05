#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Tanımlaması (Genelde adresi 0x27 olur)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Tanımlamaları
// Sağ Yol Araçları
const int sagKirmizi = 2;
const int sagSari    = 3;
const int sagYesil   = 4;

// Sol Yol Araçları
const int solKirmizi = 5;
const int solSari    = 6;
const int solYesil   = 7;

// Yayalar
const int yayaKirmizi = 8;
const int yayaYesil   = 9;

// Çevre Birimleri
const int buzzerPin = 10;
const int butonPin  = 11;

// Buton kontrolü için değişken
volatile bool butonBasildi = false;

void setup() {
  // LED Çıkışları
  pinMode(sagKirmizi, OUTPUT);
  pinMode(sagSari, OUTPUT);
  pinMode(sagYesil, OUTPUT);
  
  pinMode(solKirmizi, OUTPUT);
  pinMode(solSari, OUTPUT);
  pinMode(solYesil, OUTPUT);
  
  pinMode(yayaKirmizi, OUTPUT);
  pinMode(yayaYesil, OUTPUT);
  
  pinMode(buzzerPin, OUTPUT);
  
  // Buton Girişi (Dahili Pull-Up direnci ile)
  pinMode(butonPin, INPUT_PULLUP);

  // LCD Başlatma
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Basliyor");
  delay(1500);
}

void loop() {
  // ----------------------------------------------------------------
  // 1. AŞAMA: SAĞ YOL ARAÇLARI AKIYOR (30 Saniye)
  // ----------------------------------------------------------------
  sagYolGecisi();
  if (bekleVeButonKontrol(30, "Sag Yol: Yesil")) {
    yayaGecisFazi(); // Butona basıldıysa hemen yayaya geç
    return;
  }
  
  // Sağ yol sarı ışık
  sagYolSari();
  delay(2000);

  // ----------------------------------------------------------------
  // 2. AŞAMA: SOL YOL ARAÇLARI AKIYOR (30 Saniye)
  // ----------------------------------------------------------------
  solYolGecisi();
  if (bekleVeButonKontrol(30, "Sol Yol: Yesil")) {
    yayaGecisFazi(); // Butona basıldıysa hemen yayaya geç
    return;
  }

  // Sol yol sarı ışık
  solYolSari();
  delay(2000);

  // ----------------------------------------------------------------
  // 3. AŞAMA: OTOMATİK YAYA GEÇİŞİ (10 Saniye)
  // ----------------------------------------------------------------
  yayaGecisFazi();
}

// ================= YARDIMCI FONKSİYONLAR ================= //

void sagYolGecisi() {
  // Sağ Yeşil, Diğerleri Kırmızı
  digitalWrite(sagKirmizi, LOW);
  digitalWrite(sagSari, LOW);
  digitalWrite(sagYesil, HIGH);

  digitalWrite(solKirmizi, HIGH);
  digitalWrite(solSari, LOW);
  digitalWrite(solYesil, LOW);

  digitalWrite(yayaKirmizi, HIGH);
  digitalWrite(yayaYesil, LOW);
}

void sagYolSari() {
  digitalWrite(sagYesil, LOW);
  digitalWrite(sagSari, HIGH);
}

void solYolGecisi() {
  // Sol Yeşil, Diğerleri Kırmızı
  digitalWrite(sagKirmizi, HIGH);
  digitalWrite(sagSari, LOW);
  digitalWrite(sagYesil, LOW);

  digitalWrite(solKirmizi, LOW);
  digitalWrite(solSari, LOW);
  digitalWrite(solYesil, HIGH);

  digitalWrite(yayaKirmizi, HIGH);
  digitalWrite(yayaYesil, LOW);
}

void solYolSari() {
  digitalWrite(solYesil, LOW);
  digitalWrite(solSari, HIGH);
}

// Yayaların geçiş anı (Buzzer + LCD Geri Sayım)
void yayaGecisFazi() {
  // Tüm araçlar Kırmızı
  digitalWrite(sagKirmizi, HIGH);
  digitalWrite(sagSari, LOW);
  digitalWrite(sagYesil, LOW);

  digitalWrite(solKirmizi, HIGH);
  digitalWrite(solSari, LOW);
  digitalWrite(solYesil, LOW);

  // Yayalar Yeşil
  digitalWrite(yayaKirmizi, LOW);
  digitalWrite(yayaYesil, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("YAYALAR GECIYOR");

  // 10 Saniyelik Geri Sayım ve Buzzer
  for (int i = 10; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("Sure: ");
    lcd.print(i);
    lcd.print(" sn  ");

    if (i <= 3) {
      // Son 3 saniye: Hızlı Bip Bip (Acele et uyarısı)
      tone(buzzerPin, 1000, 150);
      delay(250);
      tone(buzzerPin, 1000, 150);
      delay(250);
      tone(buzzerPin, 1000, 150);
      delay(250);
      tone(buzzerPin, 1000, 150);
      delay(250);
    } else {
      // Normal Bip sesi
      tone(buzzerPin, 800, 300);
      delay(1000);
    }
  }

  // Yaya süresi bitti
  digitalWrite(yayaYesil, LOW);
  digitalWrite(yayaKirmizi, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lutfen Bekleyin");
  delay(1500);
}

// delay() kullanmak yerine butonu dinleyen akıllı bekleme fonksiyonu
bool bekleVeButonKontrol(int saniye, String lcdMesaji) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdMesaji);

  for (int i = saniye; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("Kalan: ");
    lcd.print(i);
    lcd.print(" sn  ");

    // Her saniyeyi 10 parçaya bölüp butonu sık sık dinliyoruz
    for (int j = 0; j < 10; j++) {
      if (digitalRead(butonPin) == LOW) { // Butona basıldı!
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Buton Algilandi!");
        lcd.setCursor(0, 1);
        lcd.print("Yaya Hazirlaniyor");
        delay(1500);
        return true; // Fonksiyondan erken çık ve yayayı başlat
      }
      delay(100);
    }
  }
  return false;
}