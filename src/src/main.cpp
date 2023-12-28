

#include <Arduino.h>

#include <Button.h>
#include <TCA9534.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Mqtt.h>
#include <Inputs.h>
#include <Outputs.h>
#include <SecretSettings.h>

TCA9534 ioex;
PCF8574 ioex_extLED;
Adafruit_SSD1306 display(64, 48, &Wire, -1);

std::vector<InputButton*> inputs = {
  new InputButton(0, true),
  new InputButton(1, true),
  new InputButton(1, true),
  new InputButton(2, true),
  new InputButton(3, true),
  new InputButton(4, true),
  new InputButton(5, true),
  new InputButton(6, true),
  new InputButton(7, true),
  new InputButton(8, true),
  new InputButton(9, true),
  new InputButton(10, true),
  new InputButton(11, true)
};
InputAnalog inpa(1);

OutputGpio outMistRemoteEn(22, false);
OutputGpio outPowerEn3v3(19, false);

std::vector<AbstractDigitalOutput*> outputs = {
  new OutputTCA9534(&ioex, 0, false),
  new OutputTCA9534(&ioex, 1, false),
  new OutputTCA9534(&ioex, 2, false),
  new OutputTCA9534(&ioex, 3, false),
  new OutputTCA9534(&ioex, 4, false),
  new OutputTCA9534(&ioex, 5, false),
  new OutputTCA9534(&ioex, 6, false),
  new OutputTCA9534(&ioex, 6, false),
  new OutputPCF8574(&ioex_extLED, 0, true),
  new OutputPCF8574(&ioex_extLED, 1, true),
  new OutputPCF8574(&ioex_extLED, 2, true),
  new OutputPCF8574(&ioex_extLED, 3, true)
};

OutputMqtt outputDustCollector("shellies/staubsauger/relay/0/command", "toggle");
InputMqtt inputDustCollector("shellies/staubsauger/relay/0", "on", "off");
OutputMqtt outputCompressor("shellies/kompressor/relay/0/command", "toggle");
InputMqtt inputCompressor("shellies/kompressor/relay/0", "on", "off");
OutputMqtt outputBlower("shellies/lüfter/relay/0/command", "toggle");
InputMqtt inputBlower("shellies/lüfter/relay/0", "on", "off");

InputButton inputPowerKey(20, false);

class MqttObserver : public Mqtt::Observer {
  /// @brief called when the connection status has changed
  /// @param status the status
  void onConnectionStatusChange(ConnectionStatus status) override {
    display.clearDisplay();
    //display.drawRoundRect(0, 0, 64, 48, 4, SSD1306_WHITE);

    display.setCursor(5, 10);
    display.print("wifi ");
    if(status.wifiConnected) {
      display.println("  ok");
    } else {
      display.println("fail");
    }

    display.setCursor(5, 30);
    display.print("mqtt ");
    if(status.mqttConnected) {
      display.println("  ok");
    } else {
      display.println("fail");
    }

    display.display();
  }
}mqttObserver;

void setup() {
  Serial.begin(9600);

  Wire.setSDA(12);
  Wire.setSCL(13);
  Wire.begin();

  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin();

  ioex.attach(Wire);
  ioex.setDeviceAddress(0x70 >> 1);
  ioex.output(0x00);

  ioex_extLED.attach(Wire1);
  ioex_extLED.setDeviceAddress(0x70 >> 1);
  ioex_extLED.output(0xFF);

  for(auto input : inputs) {
    input->begin();
  }

  for(auto output : outputs) {
    output->begin();
    output->write(1);
    delay(100);
  }

  for(auto output : outputs) {
    output->write(0);
    delay(100);
  }
  inputPowerKey.begin();
  inputDustCollector.begin();
  inputCompressor.begin();
  inputBlower.begin();

  outputDustCollector.begin();
  outputCompressor.begin();
  outputBlower.begin();

  inputPowerKey.connectOutput(outputs[8]);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false); // initialize with the I2C addr 0x3C (for the 64x48)
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();

  Mqtt::setup(WIFI_SSID, WIFI_PASSWORD, MQTT_IP, MQTT_PORT, OWN_IP);
  Mqtt::instance()->setObserver(&mqttObserver);
  

  inputs[0]->connectOutput(&outputDustCollector);
  inputs[1]->connectOutput(&outputCompressor);
  inputDustCollector.connectOutput(outputs[0]);
  inputCompressor.connectOutput(outputs[1]);
}

void loop() {
  inputPowerKey.read();
  for(auto input : inputs) {
    input->read();
  }
  Mqtt::instance()->poll();
}