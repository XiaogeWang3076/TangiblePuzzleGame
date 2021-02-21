// This example uses an Adafruit Huzzah ESP8266
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://www.shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ESP8266WiFi.h>
#include <MQTT.h>


#include <Wire.h>
#include "paj7620.h"

#define GES_REACTION_TIME    300       // You can adjust the reaction time according to the actual circumstance.
#define GES_ENTRY_TIME      800       // When you want to recognize the Forward/Backward gestures, your gestures' reaction time must less than GES_ENTRY_TIME(0.8s). 
#define GES_QUIT_TIME     1000

int LED = D5;
int i=0;

int wave;
bool Mirror_Status;
String mirrorTopic;

const char ssid[] = "iPhone";
const char pass[] = "wyh123456";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("ESPXiaoge06", "MirrorDisplay", "sd55242021")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/SequenceMirror");
  client.subscribe("/TurnOffMirror");
  client.subscribe("/RevealMirror");
  client.subscribe("/HideClue");
  client.subscribe("/EndGame");
  
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  if (String(topic) == "/SequenceMirror") {
    Mirror_Status = true;
    mirrorTopic = topic;
  }

  if (String(topic) == "/TurnOffMirror" || String(topic) == "/EndGame"){
    Mirror_Status = false;
  }

  if (String(topic) == "/RevealMirror") {
    wave = 1;
  }

   if (String(topic) == "/HideClue") {
    wave = 0;
  }
  
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  
  pinMode (LED, OUTPUT);
  
  //Gesture
  uint8_t error = 0;
  Serial.println("\nPAJ7620U2 TEST DEMO: Recognize 9 gestures.");

  error = paj7620Init();      // initialize Paj7620 registers
  if (error) 
  {
    Serial.print("INIT ERROR,CODE:"); 
    Serial.println(error);
  }
  else
  {
    Serial.println("INIT OK");
  }
  Serial.println("Please input your gestures:\n");

  
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("mqtt.eclipseprojects.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {

  if (Mirror_Status == true) {
      //提示灯 
      if (wave==0){
      for (int i = 0; i < 255; i++){ //if i is less than 255 then increase i with 1
        analogWrite(LED, i); //write the i value to pin 11
        delay(5); //wait 5 ms then do the for loop again
      }
      for (int i = 255; i > 0; i--){ //descrease i with 1
        analogWrite(LED, i);
        delay(5);
      }
  
      }
      
      if (wave == 1) {
        digitalWrite(LED, LOW);
      }
 
 
     
  //gesture

  uint8_t data = 0, data1 = 0, error;
  
  error = paj7620ReadReg(0x43, 1, &data);       // Read Bank_0_Reg_0x43/0x44 for gesture result.
  if (!error) 
  {
    switch (data)                   // When different gestures be detected, the variable 'data' will be set to different values by paj7620ReadReg(0x43, 1, &data).
    {
//      case GES_UP_FLAG:
//        delay(GES_ENTRY_TIME);
//        paj7620ReadReg(0x43, 1, &data);
//        if(data == GES_FORWARD_FLAG) 
//        {
//          wave = 0;
//          Serial.println("Forward");
//          delay(GES_QUIT_TIME);
//        }
//        else if(data == GES_BACKWARD_FLAG) 
//        {
//          wave = 0;
//          Serial.println("Backward");
//          delay(GES_QUIT_TIME);
//        }
//        else
//        {
//          wave = 0;
//          Serial.println("Up");
//        }          
//        break;
//      case GES_DOWN_FLAG:
//        delay(GES_ENTRY_TIME);
//        paj7620ReadReg(0x43, 1, &data);
//        if(data == GES_FORWARD_FLAG) 
//        {
//          wave = 0;
//          Serial.println("Forward");
//          delay(GES_QUIT_TIME);
//        }
//        else if(data == GES_BACKWARD_FLAG) 
//        {
//          wave = 0;
//          Serial.println("Backward");
//          delay(GES_QUIT_TIME);
//        }
//        else
//        {
//          wave = 0;
//          Serial.println("Down");
//        }          
//        break;
//      case GES_FORWARD_FLAG:
//        wave = 0;
//        Serial.println("Forward");
//        delay(GES_QUIT_TIME);
//        break;
//      case GES_BACKWARD_FLAG:   
//        wave = 0;  
//        Serial.println("Backward");
//        delay(GES_QUIT_TIME);
//        break;
//      case GES_CLOCKWISE_FLAG:
//        wave = 0;
//        Serial.println("Clockwise");
//        break;
//      case GES_COUNT_CLOCKWISE_FLAG:
//        wave = 0;
//        Serial.println("anti-clockwise");
//        break;  
      default:
        paj7620ReadReg(0x44, 1, &data1);
        if (data1 == GES_WAVE_FLAG) 
        {
          Serial.println("wave");
          wave = 1;
        }
        break;
    }
  }
  delay(100);

  }

  
  
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    //client.publish("/hello", "world");
    if (mirrorTopic == "/SequenceMirror") {
    String msg = "{\"gesture\":"+(String)wave+"}";
    client.publish("/mirror", msg);
    }
  }
}
