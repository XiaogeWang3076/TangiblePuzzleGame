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

const char ssid[] = "xiaoge";
const char pass[] = "qwertyuiop";

int LED = D5;
String stage2;
bool WandLight_Status;

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
  while (!client.connect("EspXiaoge01", "WandLight", "sd55242021")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/WandLight");
  client.subscribe("/TurnOnWandLight");
  client.subscribe("/EndGame");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {

  
  
  Serial.println("incoming: " + topic + " - " + payload);

  String WandLight = payload;
  if (String(topic) == "/WandLight") {
    
    Serial.print("Changing output to ");
  
    if (WandLight == "on") {

      WandLight_Status = true; //魔杖是开启状态
      Serial.println("on");
      
      stage2 = "Begin02"; //可以开启第二关
    
      for (int brightness = 0; brightness < 255; brightness++){
        analogWrite(LED, brightness);
      }
    
      digitalWrite(LED, HIGH);
    
    }
  }

  if (String(topic) == "/EndGame") {
    WandLight_Status = false;
    digitalWrite(LED, LOW);
  }

  if (String(topic) == "/TurnOnWandLight") {
//    for (int brightness = 0; brightness < 255; brightness++){
//        analogWrite(LED, brightness);
//      }
    
      digitalWrite(LED, HIGH);
  }
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.

}

void setup() {
  pinMode(D5, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("mqtt.eclipseprojects.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    
    if (WandLight_Status == true){
    client.publish("/Sequence", stage2);
    }
    
  }

  
}
