#include "Arduino.h"
#include <WiFi.h>
#include "ESP32MQTTClient.h"
#include "DHT.h"
#include <ArduinoJson.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#include <basicMPU6050.h> 

basicMPU6050<> imu;
DHT dht(DHTPIN, DHTTYPE);
const int buzzer = 2; 
const int sensor = 5;

int estadoAnterior = LOW;
int estadoActual;
bool activado = false;
bool primeraLectura = true;

const char *ssid = "UbeeDB99-2.4G";
const char *pass = "5C68EEDB99";

const char *server = "mqtt://54.147.21.63:1883";
ESP32MQTTClient mqttClient;

unsigned long lastIMUUpdate = 0; // Variable para controlar el tiempo del último update de IMU
unsigned long IMUUpdateInterval = 1000; // Intervalo de tiempo entre actualizaciones de IMU en milisegundos

void setup()
{
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, pass);
  WiFi.setHostname("c3test");
  Serial.println("Conectando a la red WiFi...");
  
  imu.setup();
  imu.setBias();
  Serial.begin(115200);

  pinMode(sensor, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  
  mqttClient.enableDebuggingMessages();
  mqttClient.setURI(server);
  mqttClient.enableLastWillMessage("lwt", "fuera de linea");
  mqttClient.setKeepAlive(30);
  mqttClient.loopStart();
}

void loop()
{
  imu.updateBias();
    if (abs(imu.gx()) > 0.20 || abs(imu.gy()) > 0.20 || abs(imu.gz()) > 0.20) {
      mqttClient.publish("/sensor/alerta/movimiento", "Alerta de movimiento", 0, false);
      Serial.println("¡Velocidad angular fuera del rango! Se ha enviado una alerta de movimiento.");
    }
    

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  estadoActual = digitalRead(sensor);
  
  // Si es la primera lectura, guardar el estado actual como estado anterior
  if (primeraLectura) {
    estadoAnterior = estadoActual;
    primeraLectura = false;
  }
  
  // Verificar si el estado actual es diferente al estado anterior
  if (estadoActual != estadoAnterior) {
    // Si el estado actual es HIGH (se separa el sensor), activar el buzzer
    if (estadoActual == HIGH) {
      if (!activado) {
        tone(buzzer, 1800);
        activado = true;
         mqttClient.publish("/alerta/robo", "alerta de seguridad se detecto una violacion en la seguridad de su bicicleta", 0, false);
      }
    } else {
      // Si el estado actual es LOW (se une el sensor), apagar el buzzer
      noTone(buzzer);
      activado = false;
    }
    // Actualizar el estado anterior al estado actual
    estadoAnterior = estadoActual;
  }
  

  float hic = dht.computeHeatIndex(t, h, false);
  StaticJsonDocument<200> doc; 

  doc["humidity"] = h;
  doc["temperature_C"] = t;
  doc["temperature_F"] = f;
  doc["heat_index"] = hic;

  // Convertir el JSON a una cadena
  char msgBuffer[256];
  serializeJson(doc, msgBuffer);
  mqttClient.publish("/prueba/123", msgBuffer, 0, false);
  delay(1000);
}

void onConnectionEstablishedCallback(esp_mqtt_client_handle_t client)
{
  if (mqttClient.isMyTurn(client))
  {
    mqttClient.subscribe("/prueba/santos", [](const String & topic, const String & payload) {
      noTone(buzzer);
    Serial.println("Mensaje recibido en el tema " + topic + ": " + payload);
    });
  }
}

esp_err_t handleMQTT(esp_mqtt_event_handle_t event)
{
  mqttClient.onEventCallback(event);
  return ESP_OK;
}
