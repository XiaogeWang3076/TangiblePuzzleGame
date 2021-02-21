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
#define GES_QUIT_TIME     500

bool BoxLock_Status;
bool Skull_Status;
bool gesture_Lock;
bool gesture_Skull;
String spell_Topic;

int LED = D7;
int i=0;

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
  while (!client.connect("ESPXiaoge08", "WandLight", "sd55242021")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/SequenceLock");
  client.subscribe("/SequenceSkull");
  client.subscribe("/TurnOffLock");
  client.subscribe("/TurnOffSkull");
  client.subscribe("/UnlockBox");
  client.subscribe("/MoveSkull");
  client.subscribe("/BoxLED");
  client.subscribe("/spells");
  client.subscribe("/EndGame");
  
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  if (String(topic) == "/SequenceLock") {
    BoxLock_Status = true;
  }

  if (String(topic) == "/TurnOffLock" || String(topic) == "/EndGame"){
    BoxLock_Status = false;
  }

  if (String(topic) == "/SequenceSkull") {
    Skull_Status = true;
  }

  if (String(topic) == "/TurnOffSkull" || String(topic) == "/EndGame"){
    Skull_Status = false;
  }

  if (String(topic) == "/spells") {
    spell_Topic = topic;
    
    
  }

  if (String(topic) == "/BoxLED") {
    gesture_Lock = false;
    gesture_Skull = false;
    
  }
  
 if (String(topic) == "/UnlockBox") {
    gesture_Lock = true;
    Serial.println("D5 High");

          digitalWrite (D3, LOW);
          digitalWrite (D4, LOW);
          digitalWrite (D5, HIGH);
          digitalWrite (D6, LOW);
          delay(6000);
  
          digitalWrite (D3, LOW);
          digitalWrite (D4, LOW);
          digitalWrite (D5, LOW);
          digitalWrite (D6, LOW);
          delay(1000);
  }

  if (String(topic) == "/MoveSkull") {
        gesture_Skull = true;
      Serial.println("D3 High");
          digitalWrite (D3, HIGH);
          digitalWrite (D4, LOW);
          digitalWrite (D5, LOW);
          digitalWrite (D6, LOW);
          delay(500);
  
          digitalWrite (D3, LOW);
          digitalWrite (D4, LOW);
          digitalWrite (D5, LOW);
          digitalWrite (D6, LOW);
          delay(500);
    
  }
  
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  
  pinMode (LED, OUTPUT);
  pinMode (D5, OUTPUT);
  pinMode (D6, OUTPUT);
  pinMode (D3, OUTPUT);
  pinMode (D4, OUTPUT);
  
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

  if (BoxLock_Status == true) {

      //提示灯 
      if (gesture_Lock == true){
        digitalWrite(LED, LOW);
      }
      
      else {
        
        for (int i = 0; i < 255; i++){ //if i is less than 255 then increase i with 1
          analogWrite(LED, i); //write the i value to pin 11
          delay(5); //wait 5 ms then do the for loop again
        }
        for (int i = 255; i > 0; i--){ //descrease i with 1
          analogWrite(LED, i);
          delay(5);
        }
      }
    
//    if (spell_Topic == "/spells"){
//      Serial.println("D5 High");
//          digitalWrite (D3, LOW);
//          digitalWrite (D4, LOW);
//          digitalWrite (D5, HIGH);
//          digitalWrite (D6, LOW);
//          delay(6000);
//  
//          digitalWrite (D3, LOW);
//          digitalWrite (D4, LOW);
//          digitalWrite (D5, LOW);
//          digitalWrite (D6, LOW);
//          delay(1000);
//
//          client.publish("/SequenceSkull", "Skull Begin");
//          client.publish("/TurnOffLock", "Turn off lock");
//    }
    
    uint8_t data = 0, data1 = 0, error;
    error = paj7620ReadReg(0x43, 1, &data);  
     
    //detect clockwise
    if (!error) {
      switch (data){
        
        case GES_DOWN_FLAG:
          delay(GES_ENTRY_TIME);
          paj7620ReadReg(0x43, 1, &data);
        
        case GES_CLOCKWISE_FLAG:
          Serial.println("Clockwise");
          gesture_Lock = true;
          client.publish("/SequenceSkull", "Skull Begin");
          Serial.println("D5 High");
          
        break;

        if (gesture_Lock == true){
          digitalWrite (D3, LOW);
          digitalWrite (D4, LOW);
          digitalWrite (D5, HIGH);
          digitalWrite (D6, LOW);
          delay(6000);
  
          digitalWrite (D3, LOW);
          digitalWrite (D4, LOW);
          digitalWrite (D5, LOW);
          digitalWrite (D6, LOW);
          delay(1000);
        }
        
      }

      //delay(2000);
    }
    
    delay(100);
  }

  
  if (Skull_Status == true) {

    //提示灯 
      if (gesture_Skull == true){
        digitalWrite(LED, LOW);
      }
      
      else {
        
        for (int i = 0; i < 255; i++){ //if i is less than 255 then increase i with 1
          analogWrite(LED, i); //write the i value to pin 11
          delay(5); //wait 5 ms then do the for loop again
        }
        for (int i = 255; i > 0; i--){ //descrease i with 1
          analogWrite(LED, i);
          delay(5);
        }
      }

    uint8_t data = 0, data1 = 0, error;
    error = paj7620ReadReg(0x43, 1, &data);

//    if (spell_Topic == "/spells"){
//      Serial.println("D3 High");
//      
//      digitalWrite (D3, HIGH);
//      digitalWrite (D4, LOW);
//      digitalWrite (D5, LOW);
//      digitalWrite (D6, LOW);
//      delay(300);
//  
//      digitalWrite (D3, LOW);
//      digitalWrite (D4, LOW);
//      digitalWrite (D5, LOW);
//      digitalWrite (D6, LOW);
//      delay(300);
//    }

    // detect Forward
  
    if (!error) {
     switch (data) {
      case GES_RIGHT_FLAG:

        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if(data == GES_FORWARD_FLAG) 
        {
          Serial.println("Forward");
            Serial.println("D3 High");

            gesture_Skull = true;
            
            digitalWrite (D3, HIGH);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
  
            digitalWrite (D3, LOW);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
            
          delay(GES_QUIT_TIME);
        }
//        else if(data == GES_BACKWARD_FLAG) 
//        {
//          Serial.println("Backward");
//          delay(GES_QUIT_TIME);
//        }
        else
        {
          Serial.println("Right");
        }          
        break;
      case GES_LEFT_FLAG: 
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if(data == GES_FORWARD_FLAG) 
        {
          Serial.println("Forward");

          Serial.println("D3 High");

            gesture_Skull = true;
            
            digitalWrite (D3, HIGH);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
  
            digitalWrite (D3, LOW);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
            
          delay(GES_QUIT_TIME);
        }
//        else if(data == GES_BACKWARD_FLAG) 
//        {
//          Serial.println("Backward");
//          delay(GES_QUIT_TIME);
//        }
        else
        {
          Serial.println("Left");
        }          
        break;
      case GES_UP_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if(data == GES_FORWARD_FLAG) 
        {
          Serial.println("Forward");
            Serial.println("D3 High");

            gesture_Skull = true;
            
            digitalWrite (D3, HIGH);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
  
            digitalWrite (D3, LOW);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
            
          delay(GES_QUIT_TIME);

          
        }
//        else if(data == GES_BACKWARD_FLAG) 
//        {
//          Serial.println("Backward");
//          delay(GES_QUIT_TIME);
//        }
        else
        {
          Serial.println("Up");
            Serial.println("D3 High");

            gesture_Skull = true;
            
            digitalWrite (D3, HIGH);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
  
            digitalWrite (D3, LOW);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
        }          
        break;
      case GES_DOWN_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if(data == GES_FORWARD_FLAG) 
        {
          Serial.println("Forward");
            
            Serial.println("D3 High");

            gesture_Skull = true;
            
            digitalWrite (D3, HIGH);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
  
            digitalWrite (D3, LOW);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);

            delay(GES_QUIT_TIME);
        }
//        else if(data == GES_BACKWARD_FLAG) 
//        {
//          Serial.println("Backward");
//          delay(GES_QUIT_TIME);
//        }
        else
        {
          Serial.println("Down");
            Serial.println("D3 High");
            gesture_Skull = true;
            
            digitalWrite (D3, HIGH);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
  
            digitalWrite (D3, LOW);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
        }          
        break;
      case GES_FORWARD_FLAG:
        Serial.println("Forward");
            Serial.println("D3 High");
            gesture_Skull = true;
            
            digitalWrite (D3, HIGH);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);
  
            digitalWrite (D3, LOW);
            digitalWrite (D4, LOW);
            digitalWrite (D5, LOW);
            digitalWrite (D6, LOW);
            delay(300);

            delay(GES_QUIT_TIME);
        break;
//      case GES_BACKWARD_FLAG:     
//        Serial.println("Backward");
//        delay(GES_QUIT_TIME);
//        break;
      case GES_CLOCKWISE_FLAG:
        Serial.println("Clockwise");
        break;
      case GES_COUNT_CLOCKWISE_FLAG:
        Serial.println("anti-clockwise");
        break;  
      
     }
    }
    
    delay(50);

  }

  
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

//  // publish a message roughly every second.
//  if (millis() - lastMillis > 1000) {
//    lastMillis = millis();
//    //client.publish("/hello", "world");
//    if (mirrorTopic == "/SequenceMirror") {
//    String msg = "{\"gesture\":"+(String)wave+"}";
//    client.publish("/mirror", msg);
//    }
//  }

}
