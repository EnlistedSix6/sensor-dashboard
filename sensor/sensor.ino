#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Configuración de WiFi
const char* ssid = "A34_F";
const char* password = "hola1234";

// Configuración del servidor
const char* serverName = "http://tu-dominio.com/api/send-data";

// Configuración del sensor DHT11
#define DHTPIN D4     // Pin digital conectado al sensor DHT11
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Intervalo de tiempo entre lecturas (en milisegundos)
const long interval = 60000;  // 60 segundos

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  
  // Iniciar el sensor DHT
  dht.begin();
  
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  Serial.println("Conectando");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a WiFi con IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
    // Guardar el último tiempo que se ejecutó esta rutina
    previousMillis = currentMillis;
    
    // Comprobar la conexión WiFi
    if(WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      
      // Leer temperatura y humedad
      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();
      
      // Comprobar si la lectura fue exitosa
      if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Error al leer del sensor DHT!");
        return;
      }
      
      // Crear objeto JSON
      DynamicJsonDocument doc(1024);
      doc["temperature"] = temperature;
      doc["humidity"] = humidity;
      
      // Serializar JSON a String
      String json;
      serializeJson(doc, json);
      
      // Enviar solicitud POST
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(json);
      
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Código HTTP: " + String(httpResponseCode));
        Serial.println("Respuesta: " + response);
      }
      else {
        Serial.print("Error en la solicitud HTTP: ");
        Serial.println(httpResponseCode);
      }
      
      // Liberar recursos
      http.end();
    }
    else {
      Serial.println("WiFi Desconectado");
    }
  }
}