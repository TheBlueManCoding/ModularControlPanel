#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoMqttClient.h>
#include <Adafruit_SSD1306.h>

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
Adafruit_SSD1306 display(64, 48, &Wire, -1);

const long interval = 10000;
unsigned long previousMillis = 0;
int count = 0;
bool state = false;

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', duplicate = ");
  Serial.print(mqttClient.messageDup() ? "true" : "false");
  Serial.print(", QoS = ");
  Serial.print(mqttClient.messageQoS());
  Serial.print(", retained = ");
  Serial.print(mqttClient.messageRetain() ? "true" : "false");
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();

  Serial.println();
}


void setup1() {
  delay(2000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.clearDisplay();
  display.drawRoundRect(0, 0, 64, 48, 4, SSD1306_WHITE);
  
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,10);
  display.println("startup");
  display.display();
  
  Serial.println("Attempting to connect to WPA SSID");
  display.setCursor(10,20);
  display.print("wifi ");
  display.display();
  while (WiFi.begin("dlink", "datpasswurt") != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  display.println("ok");
  display.display();

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.println("Attempting to connect to the MQTT broker");
  display.setCursor(10,30);
  display.print("mqtt ");
  display.display();
  if (!mqttClient.connect("192.168.0.45", 1883)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe("shellies/staubsauger/relay/0", 0);

  display.println("ok");
  display.display();
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop1() {
  mqttClient.poll();

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
      //out4.write(state);
      state = !state;
    // save the last time a message was sent
    previousMillis = currentMillis;

    //Serial.println("Sending message to topic");

    // send message, the Print interface can be used to set the message contents
    //mqttClient.beginMessage("shellies/staubsauger/relay/0/command");
    //mqttClient.print("toggle");
    //mqttClient.endMessage();

    Serial.println();

    count++;
  }
}