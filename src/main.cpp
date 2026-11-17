#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_INA219 ina219;

const char* ssid = "NARUTO SEASON 7 JANUARY";
const char* password = "bapaknyadevfor";

String apiKey = "WOJ0EOBP3PLF5011";  

const unsigned long intervalThingspeak = 20000;
unsigned long lastSend = 0;

const int pot = 5;
const int Led = 18;

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(pot, INPUT);
  Wire.begin();
  ina219.begin();
  pinMode(Led, OUTPUT);
  Serial.print("Koneksi WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
}

void loop() {
  const int potValue = analogRead(pot);
  const int value = map(potValue, 0, 4095, 0, 100);
  float arus_mA = ina219.getCurrent_mA();
  float tegangan_V = ina219.getBusVoltage_V();
   
  digitalWrite(Led, potValue >= 4000 ? HIGH : LOW);
  
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);            
  display.print("PWM: %.2f %");
  display.println(value);
  display.print("V : %.2f V\n");
  display.println(tegangan_V);
  display.print("I : %.2f mA\n");
  display.println(arus_mA);
  
  display.display();   
  if (millis() - lastSend >= intervalThingspeak) {
    lastSend = millis();

    Serial.print("Arus: ");
    Serial.print(arus_mA);
    Serial.print(" mA | Tegangan: ");
    Serial.println(tegangan_V);

    HTTPClient http;
    String url = "http://api.thingspeak.com/update?api_key=" + apiKey +
                 "&field1=" + String(value) +
                "&field2=" + String(arus_mA) +
                 "&field3=" + String(tegangan_V);

    http.begin(url);
    int httpCode = http.GET();
    String respon = http.getString();
    http.end();

    if (httpCode == 200 && respon != "0") {
      Serial.println("Data terkirim ke ThingSpeak, entry ke-" + respon);
    } else {
      Serial.println("Gagal kirim ke ThingSpeak (kode " + String(httpCode) + ")");
    }
  }
}