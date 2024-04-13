
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

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

// Thermometer
// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS D6

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int deviceCount = 0;
float tempC;

// Servo
#define SERVOPIN D5

Servo burnerServo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0;    // variable to store the servo position
int incomingByte = 0;   // for incoming serial data



// More MQTT
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (80)
char msg[MSG_BUFFER_SIZE];
int value = 0;
int t0;


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

  if (String(topic) == "destille/control") {

 //   int cmd = messageRcvd.toInt();
 //   Serial.println(cmd);
 //   burnerServo.write(cmd); 
    
    if ((char)payload[0] == '1') {
      Serial.println(" sent 0 Rotaing CW "); 
      burnerServo.write(60);
      delay(1500);
      burnerServo.write(90);
    }
    else if ((char)payload[0] == '0') {
      Serial.println(" sent 180 Rotaing CCW "); 
      burnerServo.write(120);
      delay(1500);
      burnerServo.write(90);
    }
    else if ((char)payload[0] == '2') {
      Serial.println(" sent Stopped "); 
      burnerServo.write(90);
    }

    
    
  }

}

// -----------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    // if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("hello", "I'm back");
      // ... and resubscribe
      client.subscribe("destille/control");
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
  
  // Start DS18B20s
  sensors.begin();  // Start up the library
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" DS18B20 devices.");
  Serial.println("");
  
  //Start Servo
  burnerServo.attach(SERVOPIN);  // attaches the servo on pin 9 to the servo object


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

  client.subscribe("destille/control");

}

// -----------------------------------------------
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  
  if ((millis()-t0) > 8000) {

    digitalWrite(BUILTIN_LED, LOW); 

    // Create the message that will be send using mqtt
    String message = String("actuators,room=destille ");

    // Temperature measurement
    for (int i = 0;  i < deviceCount;  i++) {
      tempC = sensors.getTempCByIndex(i);
      message += String(",temperatureSensor")+ i + "=" + String(tempC);
    }
    // Send the message on the sensors topic
    message.toCharArray(msg, message.length());
    Serial.println(msg);
  
    client.publish("sensors", msg);
    digitalWrite(BUILTIN_LED, HIGH);

    t0 = millis();
  }
}
