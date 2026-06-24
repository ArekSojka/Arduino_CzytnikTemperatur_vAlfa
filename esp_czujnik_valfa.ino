#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <HTTPClient.h>

#define ONE_WIRE_BUS 15

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Dane sieciowe
const char* ssid = "PH-WIFI";
const char* haslo = "RETAILROBOTICS";

// ThingSpeak
unsigned long channelNumber = 3411103;
const char *myWriteAPIKey = "0EL18I1OI2U27IW4";

// TalkBack
unsigned long talkBackID = 57235;
const char *talkBackAPIKey = "L5FO8NHB3A63LNJ3";

unsigned long Delay = 15000;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  sensors.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, haslo);

  Serial.print("Łączenie z siecią: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nPołączono z Wi-Fi!");
  Serial.print("Adres IP ESP32: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
}

void loop() {

  sensors.requestTemperatures();
  float temperatura = sensors.getTempCByIndex(0); //1 CZUJNIK
  float temperatura2 = sensors.getTempCByIndex(1); //2 CZUJNIK

  if (isnan(temperatura)) {
    Serial.println("Błąd odczytu temperatury!");
    temperatura = 0;
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("\n--- Wysyłanie danych do ThingSpeak ---");
    Serial.print("Temperatura: ");
    Serial.println(temperatura);
    Serial.println(temperatura2);
    ThingSpeak.setField(1, temperatura);
    ThingSpeak.setField(2, temperatura2);
    int x = ThingSpeak.writeFields(channelNumber, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Dane wysłane poprawnie!");
    } else {
      Serial.print("Błąd wysyłania. Kod HTTP: ");
      Serial.println(x);
    }

    delay(500);

    // Pobieranie komendy TalkBack
    HTTPClient httpTalkBack;
    String urlTalkBack = "http://api.thingspeak.com/talkbacks/" + String(talkBackID) +
                         "/commands/execute.json?api_key=" + String(talkBackAPIKey);

    httpTalkBack.begin(urlTalkBack);
    int httpCode = httpTalkBack.GET();

    if (httpCode == 200) {
      String odpowiedz = httpTalkBack.getString();

      int pos = odpowiedz.indexOf("\"command_string\":\"");
      if (pos != -1) {
        int start = pos + 18;
        int end = odpowiedz.indexOf("\"", start);
        String commandString = odpowiedz.substring(start, end);

        Serial.print("Odebrano komendę: ");
        Serial.println(commandString);

        float sekundy = commandString.toFloat();
        if (sekundy >= 15) {
          Delay = sekundy * 1000;
        } else {
          Serial.println("Czas zbyt mały — ThingSpeak wymaga min. 15 sekund.");
        }
      } else {
        Serial.println("Brak nowych komend TalkBack.");
      }
    } else {
      Serial.print("Błąd pobierania TalkBack. Kod HTTP: ");
      Serial.println(httpCode);
    }

    httpTalkBack.end();

  } else {
    Serial.println("Brak połączenia z Wi-Fi!");
  }

  Serial.print("Oczekiwanie: ");
  Serial.print(Delay / 1000);
  Serial.println(" sekund...");

  delay(Delay);
}
