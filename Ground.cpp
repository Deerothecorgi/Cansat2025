#include <ESP8266WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <PubSubClient.h>

 // ----- WiFi Credentials -----
const char* ssid = "your_SSID";             // Replace with  WiFi SSID
const char* password = "your_PASSWORD";     // Replace with  WiFi password

// ----- NETPIE Credentials -----
// These are provided by NETPIE when you register  device.
const char* netpieToken = "YourNetpieToken";   // Device token (used as MQTT client ID)
const char* netpieKey = "YourNetpieKey";       // Username for MQTT connection
const char* netpieSecret = "YourNetpieSecret";    // Password for MQTT connection

// NETPIE MQTT Broker Settings
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;

// Define a topic to publish data (adjust as needed)
const char* publishTopic = "/your-device/data";

// ----- LoRa Module Pins for ESP8266 -----
#define LORA_SS   D2
#define LORA_RST  D1
#define LORA_DIO0 D0

WiFiClient espClient;
PubSubClient mqttClient(espClient);

/**
 * MQTT callback function.
 * Called when a subscribed message is received.
 */
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("MQTT message received on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println("NETPIE Ground Station Starting");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());

    // Initialize LoRa module via SPI
    SPI.begin();
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(915E6)) {
        Serial.println("LoRa initialization failed!");
        while (1);
    }
    Serial.println("LoRa initialized.");

    // Initialize MQTT client for NETPIE
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(mqttCallback);
}

void reconnectMQTT() {
    // Loop until connected to NETPIE MQTT
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Connect using NETPIE credentials (client ID is the device token)
        if (mqttClient.connect(netpieToken, netpieKey, netpieSecret)) {
            Serial.println("connected");
            // Optionally subscribe to topics if needed:
            // mqttClient.subscribe("/your-device/command");
        }
        else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}

void loop() {
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }
    mqttClient.loop();

    // Check for incoming LoRa packets
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String receivedData = "";
        while (LoRa.available()) {
            receivedData += (char)LoRa.read();
        }
        Serial.println("LoRa packet: " + receivedData);

        // Publish the received data to NETPIE via MQTT
        if (mqttClient.publish(publishTopic, receivedData.c_str())) {
            Serial.println("Published to NETPIE.");
        }
        else {
            Serial.println("Publish failed.");
        }
    }
    delay(1000);
}
