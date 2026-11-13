#include <WiFi.h>
#include <HTTPClient.h>
#include <DHTesp.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* writeAPIKey = "K9SZY604JO7IJ52D";

const int DHT_PIN = 15;

DHTesp dht;

void setup() {
  Serial.begin(115200);

  dht.setup(DHT_PIN, DHTesp::DHT22);

  WiFi.begin(ssid, password, 6);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
}

void loop() {
  TempAndHumidity data = dht.getTempAndHumidity();

  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Sensor reading failed!");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(data.temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(data.humidity);
  Serial.println(" %");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url =
      "https://api.thingspeak.com/update?api_key=" +
      String(writeAPIKey) +
      "&field1=" + String(data.temperature, 1) +
      "&field2=" + String(data.humidity, 1);

    http.begin(url);
    int responseCode = http.GET();

    if (responseCode > 0) {
      String result = http.getString();
      Serial.print("ThingSpeak Entry ID: ");
      Serial.println(result);
    } else {
      Serial.println("HTTP request failed");
    }

    http.end();
  }

  delay(20000);
}