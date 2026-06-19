#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

const int pinDanych = 13;

#define DHT_PIN 13
#define DHT_TYPE DHT11

//Dane do sieci
const char* ssid = "";
const char* password = "";
const char* udpAddress = "10.10.1.34";
const int udpPort = 1234;

WiFiUDP udp;

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  delay(2000); 

  Serial.print("Łączenie z siecią.......");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi połączone!");
  Serial.print("Adres Ip ESP32L ");
  Serial.println(WiFi.localIP());
  dht.begin();
}

void loop() {
  float wilgotnosc = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (!isnan(wilgotnosc) && !isnan(temperatura)) {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" C | Wilgotnosc: ");
    Serial.print(wilgotnosc);
    Serial.println(" %");
  } else {
    Serial.println("Blad odczytu z DHT11");
  }

  delay(3000);

  udp.beginPacket(udpAddress, udpPort);
  udp.print(wilgotnosc);
  udp.print(temperatura);

  if (udp.endPacket()) {
    Serial.println("Dane IPv4 wysłane pomyślnie");
  } else {
    Serial.println("Błąd wysyłania pakietu.");
  }

  delay(2000);
}