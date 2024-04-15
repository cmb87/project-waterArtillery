#include <Arduino.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <ArduinoWebsockets.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>


#include "config.h"


ESP8266WiFiMulti wifiMulti;
// -------------------------------------------------------------------------

unsigned long t0;
unsigned long t1;
unsigned long t2;


Servo motorR;
Servo motorL;
Servo motorM;

// Websocket
using namespace websockets;
WebsocketsClient webSocket;


// -------------------------------------------------------------------------
struct CommandoStruct {
  long t;
  long x;
  long y;
  long m;
};


struct SensorStruct {
  float r;
  float h;
  float rssi;
};

SensorStruct sensorState;


// -------------------------------------------------------------------------
void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

// -------------------------------------------------------------------------
void onMessageCallback(WebsocketsMessage message) {
  // Handle WebSocket messages received from the server
  // Parse and deserialize JSON using ArduinoJson
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message.data());

  if (error) {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract data from JSON
  CommandoStruct cmds;
  cmds.t = doc["t"].as<long>();
  cmds.x = doc["x"].as<long>();
  cmds.y = doc["y"].as<long>();
  cmds.m = doc["m"].as<long>();

  // Parse the message data into the struct
  Serial.print("t: ");
  Serial.print(cmds.t);
  Serial.print(" x:");
  Serial.print(cmds.x);
  Serial.print(" y:");
  Serial.print(cmds.y);
  Serial.print(" m:");
  Serial.print(cmds.m);
  //Serial.println();

  if (cmds.t == 0) {

    unsigned int steer =    map(cmds.x,-255,255,1000,2000);
    unsigned int throttle = map(abs(cmds.y),0,255,1000,2000);

    unsigned int motorMPwm = map(abs(cmds.m),0,255,1000,2000);
    unsigned int motorRPwm = constrain(throttle - 2*(steer-1500),1000,2000);
    unsigned int motorLPwm = constrain(throttle + 2*(steer-1500),1000,2000);

    motorR.writeMicroseconds(motorRPwm);
    motorL.writeMicroseconds(motorLPwm);
    motorM.writeMicroseconds(motorMPwm);


    Serial.print(" pwmR: ");
    Serial.print(motorRPwm);
    Serial.print(" pwmL:");
    Serial.print(motorLPwm);
    Serial.print(" pwmM:");
    Serial.print(motorMPwm);
    Serial.println();

  } 
  else if (cmds.t == 1) {
    digitalWrite(BUILTIN_LED, HIGH); 
    Serial.println("Lights on");
  }
  else if (cmds.t == 2) {
    digitalWrite(BUILTIN_LED, LOW); 
    Serial.println("Lights off");
  }
}

// -------------------------------------------------------------------------
void sendJsonData() {
  // Create a JSON document using ArduinoJson
  StaticJsonDocument<256> doc;
  doc["r"] = sensorState.r;
  doc["h"] = sensorState.h;
  doc["rssi"] = sensorState.rssi;
  // Convert the JSON document to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send the JSON string over the WebSocket
  webSocket.send(jsonString);
}

// -------------------------------------------------------------------------
void connectWebSocket() {

  Serial.print("Connecting to ");
  Serial.println(WEBSOCKET_SERVER);

  // Set up the WebSocket client
  webSocket.onEvent(onEventsCallback);
  webSocket.onMessage(onMessageCallback);

  if (webSocket.connect(WEBSOCKET_SERVER)) {
    Serial.println("WebSocket connected!");
  } else {
    Serial.println("WebSocket connection failed!");
  }

}

// -------------------------------------------------------------------------
void setup() {

  Serial.begin(115200);

  // ---------------- Wifi ----------------
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
  //wifiMulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);

  Serial.println("Connecting to Wi-Fi");
 
  // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // ---------------- Websocket ----------------
  connectWebSocket();

  // ---------------- Timers ----------------
  t0 = millis();
  t1 = millis();
  t2 = millis();

  // ---------------- Light ----------------
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output


  // ---------------- Motors ----------------
  motorR.attach(MOTOR_R_PIN);
  motorL.attach(MOTOR_L_PIN);
  motorM.attach(MOTOR_M_PIN);

  motorR.writeMicroseconds(MAX_SIGNAL);
  motorL.writeMicroseconds(MAX_SIGNAL);
  motorM.writeMicroseconds(MAX_SIGNAL);

  motorR.writeMicroseconds(MIN_SIGNAL);
  motorL.writeMicroseconds(MIN_SIGNAL);
  motorM.writeMicroseconds(MIN_SIGNAL);


}


// -------------------------------------------------------------------------
void loop() {
   
  // let the websockets client check for incoming messages
  if(webSocket.available()) {
    webSocket.poll();
  }

  // State Publish
  if ( millis()-t0 > 5000 ) {
    sensorState.r = 100;
    sensorState.h = 90;
    sensorState.rssi = WiFi.RSSI();
    sendJsonData();
    t0 = millis();
  }

}
 
