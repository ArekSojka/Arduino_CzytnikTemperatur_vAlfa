#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C led(0x27, 16, 2);
#define DHT_PIN A0
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  led.init();
  led.backlight();
  led.setCursor(0,0);

  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float wilgotnosc = dht.readHumidity();
  float temperatura = dht.readTemperature();


  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" C");

  Serial.print("Wilgotnosc: ");
  Serial.print(wilgotnosc);
  Serial.println(" %");

  led.setCursor(0,0);
  led.print(temperatura);
  led.print(" C");
  led.setCursor(0,1);
  led.print(wilgotnosc);
  led.print(" %");

  delay(2000);

}
