#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include "ArduinoJson.h"
#include "config.h"


// https://lastminuteengineers.com/multiple-ds18b20-arduino-tutorial/
// Update these with values suitable for your network.

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* mqttServer = MQTT_BROKER_NAME;
const int mqttPort = MQTT_BROKER_PORT;
const char* mqttUser = MQTT_BROKER_USER;
const char* mqttPassword = MQTT_BROKER_PASSWORD;


WiFiClient espClient;
PubSubClient client(espClient);



// RELAY
#define RELAYPIN D6
#define SERVO0PIN D7
#define SERVO1PIN D8

Servo servo0;  
Servo servo1;  

int minAngle0 = 0;
int maxAngle0 = 180;

int minAngle1 = 0;
int maxAngle1 = 180;


int pos1 =  0;
int pos2 =  0;
int relay = 0;
long t0;

// More MQTT
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (80)
char msg[MSG_BUFFER_SIZE];
int value = 0;



// ==============================================
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// -----------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String messageRcvd;

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageRcvd += (char)payload[i];
  }

  Serial.println("");

  if (String(topic) == "turret/control") {

    // Parse JSON
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);
    
    // Set servo positions
    pos1 =  doc["x"];
    pos2 =  doc["y"];
    relay = doc["r"];

    pos1 = constrain(pos1, minAngle0, maxAngle0);
    pos2 = constrain(pos2, minAngle1, maxAngle1);

    servo0.write(pos1);
    servo1.write(pos2);

    if (relay == 1) {
      digitalWrite(RELAYPIN, HIGH);
      digitalWrite(BUILTIN_LED, LOW); 
    } else {
      digitalWrite(RELAYPIN, LOW); // Turn off the relay
      digitalWrite(BUILTIN_LED, HIGH); 
    } // Turn on the relay
    
  }

}

// -----------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    // if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
    if (client.connect("ESP8266Client-Waterturret", mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("hello", "I'm back");
      // ... and resubscribe
      client.subscribe("turret/control");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// -----------------------------------------------
void setup() {
  
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(115200);
  


  //Start Servo
  servo0.attach(SERVO0PIN);  // attaches the servo on pin 9 to the servo object
  servo1.attach(SERVO1PIN);  // attaches the servo on pin 9 to the servo object

  pinMode(RELAYPIN, OUTPUT); // Set relay pin as an output

  // Wifi and MQTT
  setup_wifi();
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  digitalWrite(BUILTIN_LED, HIGH); 
  delay(2000);
  digitalWrite(BUILTIN_LED, LOW); 
  
  if (!client.connected()) {
    reconnect();
  }

  t0 = millis();
  pos1 = servo0.read();
  pos2 = servo1.read();


}

// -----------------------------------------------
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  if (millis()-t0 > 2000) {
    // Create a JSON message with feedback data
    DynamicJsonDocument doc(256);
    doc["servo0"] = servo0.read();
    doc["servo1"] = servo1.read();
    doc["relay"] = relay;
    // Convert message to a char array
    char msgBuffer[256];
    serializeJson(doc, msgBuffer);

    // Publish the message to the feedback topic
    client.publish("turret/status", msgBuffer);
    Serial.println("printed");
    t0 = millis();
  }
  
}
