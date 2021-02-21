#include <ESP8266WiFi.h>
#include <MQTT.h>

const char ssid[] = "xiaoge";
const char pass[] = "qwertyuiop";

int light = 0;
int LED1 = D7; 
bool Calendar1_Status;

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
  while (!client.connect("EspXiaoge03", "WandLight", "sd55242021")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/Sequence");
  client.subscribe("/TurnOffCalendar");
  client.subscribe("/EndGame");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  
  if (String(topic) == "/Sequence") {
    
    Serial.print("The stage 02 begins");
    Calendar1_Status = true;
    
   }

   if (String(topic) == "/TurnOffCalendar" || String(topic) == "/EndGame") {
    Serial.print("The stage 02 stops");
    Calendar1_Status = false;
   }

}

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.

void setup() {

  pinMode (D7, OUTPUT);
  pinMode (D0, OUTPUT);
  
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("mqtt.eclipseprojects.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {

  if (Calendar1_Status == true){
    
     //Turn on photo resistor
    digitalWrite(D0,HIGH);
    
    //Read light value
    light = analogRead(A0);
    Serial.println(light);

    if(light > 500) {
      Serial.println("It is quite light!");
      digitalWrite(LED1,HIGH);
    }
    else{
      Serial.println("It is average light");
      digitalWrite(LED1,LOW);
    }
  }

  if (Calendar1_Status == false) {
    //Turn off photo resistor
    digitalWrite(D0, LOW);
  }
  
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
//  if (Calendar1_Status == true;){
//    if (millis() - lastMillis > 1000) {
//      lastMillis = millis();
//      client.publish("/CalendarStatus", Calendar1_Status);
//     }
//  }

}
