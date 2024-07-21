#include "WiFi.h"
#include <HTTPClient.h>

// Masukkan SSID (Nama WiFi) dan Password
const char* ssid = ""; // Ganti dengan SSID WiFi Anda
const char* password = ""; // Ganti dengan password WiFi Anda

// Masukkan token bot Telegram dan chat ID Anda
const char* telegramToken = ""; // Ganti dengan token bot Telegram Anda
const char* chatID = ""; // Ganti dengan chat ID Anda

// Pin Definitions
const int MQ2_DIGITAL_PIN = 13;      // Pin sensor asap (digital DOUT) di GPIO13
const int FLAME_SENSOR_PIN = 35;     // Pin sensor api (digital)
const int BUZZER_PIN = 4;            // Pin buzzer
const int LED_PIN = 5;               // Pin LED

bool buzzerActive = false;           // Untuk melacak apakah buzzer menyala

void setup() {
  Serial.begin(115200);              // Inisialisasi komunikasi serial
  pinMode(MQ2_DIGITAL_PIN, INPUT);   // Set pin sensor asap sebagai input digital
  pinMode(FLAME_SENSOR_PIN, INPUT);  // Set pin sensor api sebagai input
  pinMode(BUZZER_PIN, OUTPUT);       // Set pin buzzer sebagai output
  pinMode(LED_PIN, OUTPUT);          // Set pin LED sebagai output
  digitalWrite(BUZZER_PIN, LOW);     // Matikan buzzer awalnya
  digitalWrite(LED_PIN, LOW);        // Matikan LED awalnya

  // Sambungkan ke WiFi
  WiFi.begin(ssid, password);
  int reset = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    reset++;
    Serial.print("Menyambungkan ke WiFi : ");
    Serial.println(ssid);
    if (reset == 5) {
      ESP.restart();
    }
  }
  Serial.print("Berhasil Menyambungkan ke WiFi : ");
  Serial.println(ssid);
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int smokeDetected = digitalRead(MQ2_DIGITAL_PIN);  // Baca nilai DOUT dari GPIO13 (digital)
  int flameDetected = digitalRead(FLAME_SENSOR_PIN); // Baca nilai sensor api dari GPIO35

  Serial.print("Smoke Detected (DOUT): ");
  Serial.println(smokeDetected);
  Serial.print("Flame Detected: ");
  Serial.println(flameDetected);

  // Cek apakah ada asap atau api terdeteksi
  if (smokeDetected == LOW || flameDetected == LOW) {  // Sensor digital aktif rendah (LOW) menandakan deteksi
    digitalWrite(BUZZER_PIN, HIGH); // Nyalakan buzzer
    digitalWrite(LED_PIN, HIGH);    // Nyalakan LED

    if (!buzzerActive) {
      Serial.println("WARNING: Terdeteksi Adanya Kebakaran!");
      sendTelegramMessage("Warning!!! Terdeteksi Adanya Kebakaran");
      buzzerActive = true; // Tandai bahwa buzzer sudah menyala dan notifikasi dikirim
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Matikan buzzer
    digitalWrite(LED_PIN, LOW);     // Matikan LED
    buzzerActive = false;           // Reset status buzzer
  }

  delay(1000); // Delay 1 detik sebelum membaca kembali
}

void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + String(telegramToken) + "/sendMessage?chat_id=" + String(chatID) + "&text=" + message;
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}
