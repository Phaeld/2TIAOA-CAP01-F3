/*
Project: CardioIA - Part 2 (Fog/Cloud Computing)
Platform: ESP32
Environment: Wokwi / Arduino IDE
Author: Raphael da Silva RM: 561452
Group: São Paulo e Interior

Objective:
- Read/simulate health data from ESP32
- Send data to cloud using MQTT
- Use HiveMQ Cloud as MQTT broker
- Display data in Node-RED Dashboard
- Generate alerts for abnormal temperature, BPM or emergency button
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <Wire.h>
#include <MPU6050.h>

// =====================================================
// WI-FI CONFIGURATION
// =====================================================

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// =====================================================
// MQTT CONFIGURATION - HIVEMQ CLOUD
// =====================================================

const char* MQTT_SERVER = "4608f563daee472baeab8f89f26230b9.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;

const char* MQTT_USER = "phaeld";
const char* MQTT_PASSWORD = "cap01F3@";


const char* MQTT_CLIENT_ID = "cardioia-esp32-client-01";

// Main topic used by Node-RED
const char* MQTT_TOPIC_HEALTH = "cardioia/health/data";

// Status topic
const char* MQTT_TOPIC_STATUS = "cardioia/health/status";

// =====================================================
// PINOUT AND CONFIGURATIONS
// =====================================================

// DHT22 sensor
static const uint8_t DHT_PIN = 15;

// Potentiometer used to simulate heart rate
static const uint8_t HEART_RATE_PIN = 34;

// Emergency button
static const uint8_t EMERGENCY_BUTTON_PIN = 4;

// Local alert LED
static const uint8_t ALERT_LED_PIN = 13;

// I2C - MPU6050
static const uint8_t MPU_SDA_PIN = 21;
static const uint8_t MPU_SCL_PIN = 22;

// =====================================================
// TIMING CONFIGURATION
// =====================================================

static const uint32_t SENSOR_READ_INTERVAL_MS = 2000;
static const uint32_t MQTT_RECONNECT_INTERVAL_MS = 5000;

// =====================================================
// ALERT THRESHOLDS
// =====================================================

static const float TEMPERATURE_ALERT_C = 38.0f;
static const uint8_t HEART_RATE_ALERT_BPM = 120;

// =====================================================
// OBJECTS
// =====================================================

DHTesp dhtSensor;
MPU6050 mpuSensor;

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

// =====================================================
// DATA STRUCTURE
// =====================================================

struct HealthRecord {
  uint32_t timestampMs;
  float temperatureC;
  float humidityPercent;
  uint8_t heartRateBpm;
  bool movementDetected;
  bool emergencyPressed;
  bool alertActive;
};

// =====================================================
// GLOBAL VARIABLES
// =====================================================

uint32_t lastSensorReadMs = 0;
uint32_t lastMqttReconnectAttemptMs = 0;

float currentTemperatureC = 0.0f;
float currentHumidityPercent = 0.0f;
uint8_t currentHeartRateBpm = 0;
bool currentMovementDetected = false;
bool currentEmergencyPressed = false;
bool currentAlertActive = false;

// =====================================================
// PROTOTYPES
// =====================================================

// Initialization
void initializePins();
void initializeSensors();
void initializeWifi();
void initializeMqtt();
void initializeSystem();

// Connectivity
void connectToWifi();
bool connectToMqtt();
void maintainMqttConnection();

// Sensor reading
void readDhtValues(float &temperatureC, float &humidityPercent);
uint8_t readHeartRate();
bool readMovement();
bool readEmergencyButton();

// Processing
HealthRecord createHealthRecord();
void processCurrentReading();
bool evaluateAlerts(const HealthRecord &record);

// MQTT
void publishHealthRecord(const HealthRecord &record);
String createJsonPayload(const HealthRecord &record);

// Utilities
void printCurrentReading(const HealthRecord &record);
const char* boolToText(bool value);

// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(500);

  initializeSystem();

  Serial.println("======================================");
  Serial.println("CardioIA - Part 2 MQTT Cloud started");
  Serial.println("======================================");
}

// =====================================================
// LOOP
// =====================================================

void loop() {
  const uint32_t currentMs = millis();

  // Keeps Wi-Fi and MQTT connection alive
  maintainMqttConnection();
  mqttClient.loop();

  // Periodic sensor reading and MQTT publishing
  if (currentMs - lastSensorReadMs >= SENSOR_READ_INTERVAL_MS) {
    lastSensorReadMs = currentMs;
    processCurrentReading();
  }
}

// =====================================================
// INITIALIZATION
// =====================================================

void initializePins() {
  pinMode(HEART_RATE_PIN, INPUT);

  pinMode(EMERGENCY_BUTTON_PIN, INPUT_PULLUP);

  pinMode(ALERT_LED_PIN, OUTPUT);
  digitalWrite(ALERT_LED_PIN, LOW);
}

void initializeSensors() {
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  Wire.begin(MPU_SDA_PIN, MPU_SCL_PIN);
  mpuSensor.initialize();

  if (mpuSensor.testConnection()) {
    Serial.println("[MPU6050] Sensor initialized successfully.");
  } else {
    Serial.println("[MPU6050] Sensor initialization failed.");
  }
}

void initializeWifi() {
  connectToWifi();
}

void initializeMqtt() {
  secureClient.setInsecure();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setKeepAlive(60);
}

void initializeSystem() {
  initializePins();
  initializeSensors();
  initializeWifi();
  initializeMqtt();
}

// =====================================================
// WI-FI AND MQTT
// =====================================================

void connectToWifi() {
  Serial.println("[WiFi] Connecting...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("[WiFi] Connected successfully.");
  Serial.print("[WiFi] IP address: ");
  Serial.println(WiFi.localIP());
}

bool connectToMqtt() {
  if (mqttClient.connected()) {
    return true;
  }

  Serial.println("[MQTT] Connecting to broker...");

  bool connected = mqttClient.connect(
    MQTT_CLIENT_ID,
    MQTT_USER,
    MQTT_PASSWORD
  );

  if (connected) {
    Serial.println("[MQTT] Connected successfully.");
    Serial.print("[MQTT] Client ID: ");
    Serial.println(MQTT_CLIENT_ID);

    mqttClient.publish(
      MQTT_TOPIC_STATUS,
      "{\"status\":\"connected\",\"device\":\"CardioIA ESP32\"}",
      true
    );

    return true;
  }

  Serial.print("[MQTT] Connection failed. State: ");
  Serial.println(mqttClient.state());

  return false;
}

void maintainMqttConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWifi();
  }

  if (!mqttClient.connected()) {
    const uint32_t currentMs = millis();

    if (currentMs - lastMqttReconnectAttemptMs >= MQTT_RECONNECT_INTERVAL_MS) {
      lastMqttReconnectAttemptMs = currentMs;
      connectToMqtt();
    }
  }
}

// =====================================================
// SENSOR READINGS
// =====================================================

void readDhtValues(float &temperatureC, float &humidityPercent) {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  temperatureC = data.temperature;
  humidityPercent = data.humidity;

  if (isnan(temperatureC) || isnan(humidityPercent)) {
    Serial.println("[DHT22] Failed reading sensor values.");
    temperatureC = -1.0f;
    humidityPercent = -1.0f;
  }
}

uint8_t readHeartRate() {
  const int analogValue = analogRead(HEART_RATE_PIN);

  const long mappedValue = map(analogValue, 0, 4095, 60, 140);

  Serial.print("[HEART] Analog value: ");
  Serial.print(analogValue);
  Serial.print(" | BPM: ");
  Serial.println(mappedValue);

  return (uint8_t)mappedValue;
}

bool readMovement() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpuSensor.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  const int16_t movementThreshold = 2000;

  if (abs(ax) > movementThreshold ||
      abs(ay) > movementThreshold ||
      abs(az) > movementThreshold) {
    return true;
  }

  return false;
}

bool readEmergencyButton() {
  return digitalRead(EMERGENCY_BUTTON_PIN) == LOW;
}

// =====================================================
// RECORD CREATION AND PROCESSING
// =====================================================

HealthRecord createHealthRecord() {
  HealthRecord record;

  record.timestampMs = millis();
  record.temperatureC = currentTemperatureC;
  record.humidityPercent = currentHumidityPercent;
  record.heartRateBpm = currentHeartRateBpm;
  record.movementDetected = currentMovementDetected;
  record.emergencyPressed = currentEmergencyPressed;
  record.alertActive = currentAlertActive;

  return record;
}

void processCurrentReading() {
  // Read current sensor values
  readDhtValues(currentTemperatureC, currentHumidityPercent);
  currentHeartRateBpm = readHeartRate();
  currentMovementDetected = readMovement();
  currentEmergencyPressed = readEmergencyButton();

  // Create current health record
  HealthRecord record = createHealthRecord();

  // Evaluate alert locally, representing the Fog/Edge layer
  currentAlertActive = evaluateAlerts(record);
  record.alertActive = currentAlertActive;

  // Local alert output
  digitalWrite(ALERT_LED_PIN, currentAlertActive ? HIGH : LOW);

  // Serial monitor debug
  printCurrentReading(record);

  // Publish to cloud via MQTT
  if (mqttClient.connected()) {
    publishHealthRecord(record);
  } else {
    Serial.println("[MQTT] Not connected. Data was not published.");
  }
}

bool evaluateAlerts(const HealthRecord &record) {
  bool alertActive = false;

  if (record.temperatureC > TEMPERATURE_ALERT_C) {
    alertActive = true;
    Serial.println("[ALERT] Temperature above safe limit.");
  }

  if (record.heartRateBpm > HEART_RATE_ALERT_BPM) {
    alertActive = true;
    Serial.println("[ALERT] Heart rate above safe limit.");
  }

  if (record.emergencyPressed) {
    alertActive = true;
    Serial.println("[CRITICAL ALERT] Emergency button pressed.");
  }

  return alertActive;
}

// =====================================================
// MQTT PUBLISHING
// =====================================================

void publishHealthRecord(const HealthRecord &record) {
  String payload = createJsonPayload(record);

  bool published = mqttClient.publish(MQTT_TOPIC_HEALTH, payload.c_str());

  if (published) {
    Serial.println("[MQTT] Data published successfully.");
    Serial.print("[MQTT] Topic: ");
    Serial.println(MQTT_TOPIC_HEALTH);
    Serial.print("[MQTT] Payload: ");
    Serial.println(payload);
  } else {
    Serial.println("[MQTT] Failed to publish data.");
  }
}

String createJsonPayload(const HealthRecord &record) {
  String json = "{";

  json += "\"timestamp_ms\":";
  json += record.timestampMs;
  json += ",";

  json += "\"temperature_c\":";
  json += String(record.temperatureC, 2);
  json += ",";

  json += "\"humidity_percent\":";
  json += String(record.humidityPercent, 2);
  json += ",";

  json += "\"heart_rate_bpm\":";
  json += record.heartRateBpm;
  json += ",";

  json += "\"movement_detected\":";
  json += record.movementDetected ? "true" : "false";
  json += ",";

  json += "\"emergency_pressed\":";
  json += record.emergencyPressed ? "true" : "false";
  json += ",";

  json += "\"alert_active\":";
  json += record.alertActive ? "true" : "false";

  json += "}";

  return json;
}

// =====================================================
// UTILITIES
// =====================================================

void printCurrentReading(const HealthRecord &record) {
  Serial.println("===== CURRENT READING =====");

  Serial.print("Temperature (C): ");
  Serial.println(record.temperatureC, 2);

  Serial.print("Humidity (%): ");
  Serial.println(record.humidityPercent, 2);

  Serial.print("Heart Rate (BPM): ");
  Serial.println(record.heartRateBpm);

  Serial.print("Movement: ");
  Serial.println(boolToText(record.movementDetected));

  Serial.print("Emergency: ");
  Serial.println(boolToText(record.emergencyPressed));

  Serial.print("Alert: ");
  Serial.println(boolToText(record.alertActive));

  Serial.println("===========================");
}

const char* boolToText(bool value) {
  return value ? "true" : "false";
}