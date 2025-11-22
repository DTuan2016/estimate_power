// #include <ESP8266WiFi.h>
// #include <SoftwareSerial.h>
// #include <PZEM004Tv30.h>

// // Thay WiFi nhà bạn
// const char* ssid = "FutureInternetLab";
// const char* password = "labchungminh618";

// // SoftwareSerial cho PZEM
// #define PZEM_RX 15  // D7
// #define PZEM_TX 13  // D8
// SoftwareSerial pzemSW(PZEM_RX, PZEM_TX);
// PZEM004Tv30 pzem(pzemSW);

// WiFiServer server(80);

// void setup() {
//   pzemSW.begin(9600);      // UART PZEM
//   delay(1000);             // chờ PZEM ổn định

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//   }

//   server.begin();
// }

// float voltage, current, power, energy, freq, pf;
// unsigned long lastRead = 0;

// void loop() {
//   // Đọc PZEM mỗi 1 giây
//   if (millis() - lastRead > 2000) {
//     voltage = pzem.voltage(); if(isnan(voltage)) voltage=0;
//     current = pzem.current(); if(isnan(current)) current=0;
//     power   = pzem.power();   if(isnan(power))   power=0;
//     energy  = pzem.energy();  if(isnan(energy))  energy=0;
//     freq    = pzem.frequency(); if(isnan(freq)) freq=0;
//     pf      = pzem.pf(); if(isnan(pf)) pf=0;
//     lastRead = millis();
//   }

//   WiFiClient client = server.available();
//   if(!client) return;
//   while(!client.available()) delay(1);
//   String req = client.readStringUntil('\r'); client.flush();

//   if(req.indexOf("GET /data")!=-1){
//     client.println("HTTP/1.1 200 OK");
//     client.println("Content-Type: application/json");
//     client.println("Connection: close");
//     client.println();
//     client.printf("{\"voltage\":%.2f,\"current\":%.2f,\"power\":%.2f,"
//                   "\"energy\":%.2f,\"frequency\":%.2f,\"pf\":%.2f}\n",
//                   voltage, current, power, energy, freq, pf);

//     client.flush();
//     delay(1000);  // tăng delay để chắc chắn TCP gửi xong
//     client.stop();
//   }
// }
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>

// WiFi nhà bạn
const char* ssid = "FutureInternetLab";
const char* password = "labchungminh618";

// Địa chỉ server nhận dữ liệu
const char* serverURL = "http://192.168.10.18:5000/pzem"; // ví dụ server PC chạy Flask

// SoftwareSerial cho PZEM
#define PZEM_RX 15  // D7
#define PZEM_TX 13  // D8
SoftwareSerial pzemSW(PZEM_RX, PZEM_TX);
PZEM004Tv30 pzem(pzemSW);

// Thời gian giữa các lần gửi
const unsigned long interval = 1000; // 1 giây
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  pzemSW.begin(9600);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - lastSend >= interval) {
    lastSend = millis();

    // Đọc PZEM
    float voltage = pzem.voltage();     if (isnan(voltage)) voltage = 0.0;
    float current = pzem.current();     if (isnan(current)) current = 0.0;
    float power   = pzem.power();       if (isnan(power))   power = 0.0;
    float energy  = pzem.energy();      if (isnan(energy))  energy = 0.0;
    float freq    = pzem.frequency();   if (isnan(freq))    freq = 0.0;
    float pf      = pzem.pf();          if (isnan(pf))      pf = 0.0;

    // Gửi POST về server
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverURL); // <-- sửa ở đây
      http.addHeader("Content-Type", "application/json");

      String payload = "{";
      payload += "\"voltage\":" + String(voltage) + ",";
      payload += "\"current\":" + String(current) + ",";
      payload += "\"power\":"   + String(power) + ",";
      payload += "\"energy\":"  + String(energy) + ",";
      payload += "\"frequency\":" + String(freq) + ",";
      payload += "\"pf\":"        + String(pf);
      payload += "}";

      int httpResponseCode = http.POST(payload);
      http.end();
    }
  }
}

