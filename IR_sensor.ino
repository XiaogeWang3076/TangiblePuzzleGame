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

int IRSensor = D5;
bool statusSensor;
String WandLight;

//wifi
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
  while (!client.connect("EspXiaoge02", "WandLight", "sd55242021")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  
  client.subscribe("/Start");
  client.subscribe("/EndGame");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  if (String(topic) == "/Start") {
      statusSensor = true;
  }
  else {
    statusSensor = false;
  }


  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {

  pinMode(IRSensor, INPUT);
  
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("mqtt.eclipseprojects.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {

  int IRSensor_Value = digitalRead (IRSensor);
  if (statusSensor == true){
      if (IRSensor_Value == 0){
        WandLight="on";
        }
      else{
        WandLight="off";
      }

     Serial.println(WandLight);
  }
  
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();

    if (statusSensor == true){
    client.publish("/WandLight", WandLight);
    }
    
  }
  
}
