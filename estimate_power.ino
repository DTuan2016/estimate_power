#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>

const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";

const char* serverURL = "http://<SERVER_IP>:5000/pzem";

#define PZEM_RX 15  // D7
#define PZEM_TX 13  // D8
SoftwareSerial pzemSW(PZEM_RX, PZEM_TX);
PZEM004Tv30 pzem(pzemSW);

const unsigned long interval = 1000; 
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

    // Read PZEM valuier
    float voltage = pzem.voltage();     if (isnan(voltage)) voltage = 0.0;
    float current = pzem.current();     if (isnan(current)) current = 0.0;
    float power   = pzem.power();       if (isnan(power))   power = 0.0;
    float energy  = pzem.energy();      if (isnan(energy))  energy = 0.0;
    float freq    = pzem.frequency();   if (isnan(freq))    freq = 0.0;
    float pf      = pzem.pf();          if (isnan(pf))      pf = 0.0;

    // Sent POST to server
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverURL);
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

