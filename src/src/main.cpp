
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include <Button.h>
#include <TCA9534.h>
#include <Wire.h>
#include <WifiClient.h>

WiFiClient wifiClient;

Adafruit_SSD1306 display(64, 48, &Wire, -1);

TCA9534 ioex;
const uint8_t IOEX_ADDR = 0x70 >> 1; // A0 = A1 = A2 = 0

Button powerButton(20);

const int POWER_EN_3V3 = 19;

void setup()
{
  Serial.begin(9600);
  Serial.println("startup"); 
  
  powerButton.begin();

  Wire.setSDA(12);
  Wire.setSCL(13);

  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire.begin();
  Wire1.begin();

  ioex.attach(Wire);
  ioex.setDeviceAddress(IOEX_ADDR);
  ioex.output(0x00);
  ioex.config(TCA9534::Config::OUT);
  pinMode(POWER_EN_3V3, OUTPUT); 
 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.clearDisplay();
  display.drawRoundRect(0, 0, 64, 48, 4, SSD1306_WHITE);
  display.setCursor(10,10);
  display.setTextColor(SSD1306_WHITE);
  display.println("startup");
  display.display();
  
}

void loop()
{
  if (powerButton.pressed()) {
    ioex.output(0, TCA9534::Level::H);
    digitalWrite(POWER_EN_3V3, HIGH);
    Serial.println("powerButton 1 pressed");
  }

  if (powerButton.released()) {
    ioex.output(0, TCA9534::Level::L);
    digitalWrite(POWER_EN_3V3, LOW);
  }

}