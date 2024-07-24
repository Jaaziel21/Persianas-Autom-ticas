#include <WiFi.h>
#include <PubSubClient.h>
#include <AccelStepper.h>

// Definiciones del motor a pasos
#define DIR_PIN 5
#define STEP_PIN 4
#define ENABLE_PIN 14

#define MQTT_SERVER "broker.hivemq.com"  // Public MQTT broker for testing
#define MQTT_PORT 1883
#define MQTT_USER ""  // No user for public broker
#define MQTT_PASSWORD ""  // No password for public broker

const char* ssid = "Pezcadoo_31";
const char* password = "Avaesva_31";
const char* topic = "home/blinds";

WiFiClient espClient;
PubSubClient client(espClient);
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  // Configuración del motor a pasos
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);  // Habilita el motor
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String command;
  for (unsigned int i = 0; i < length; i++) {
    command += (char)payload[i];
  }

  Serial.print("Received message: ");
  Serial.println(command);

  if (command == "up") {
    moveBlindsUp();
  } else if (command == "down") {
    moveBlindsDown();
  } else if (command == "stop") {
    stopBlinds();
  }
}

void moveBlindsUp() {
  stepper.moveTo(1000);  // Ajusta este valor según el recorrido necesario
  stepper.runToPosition();
}

void moveBlindsDown() {
  stepper.moveTo(0);  // Ajusta este valor según el recorrido necesario
  stepper.runToPosition();
}

void stopBlinds() {
  stepper.stop();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
