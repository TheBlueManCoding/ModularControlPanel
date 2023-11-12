

#include <Arduino.h>

#include <Button.h>
#include <TCA9534.h>
#include <Wire.h>
#include <PCF8574.h>
#include <CoreMutex.h>

#include <Inputs.h>
#include <Outputs.h>

mutex_t mutex;
CoreMutex cMutex(&mutex);

TCA9534 ioex;
PCF8574 ioex_extLED;

InputGpio inp(0);
InputAnalog inpa(1);

OutputGpio outMistRemoteEn(22, false);
OutputGpio outPowerEn3v3(19, false);
OutputTCA9534 outLed1(&ioex, 0, false);
OutputTCA9534 outLed2(&ioex, 1, false);
OutputTCA9534 outLed3(&ioex, 2, false);
OutputTCA9534 outLed4(&ioex, 3, false);
OutputTCA9534 outLed5(&ioex, 4, false);
OutputTCA9534 outLed6(&ioex, 5, false);
OutputTCA9534 outLed7(&ioex, 6, false);
OutputTCA9534 outLed8(&ioex, 7, false);
OutputPCF8574 outExtLed1(&ioex_extLED, 0, true);
OutputPCF8574 outExtLed2(&ioex_extLED, 1, true);
OutputPCF8574 outExtLed3(&ioex_extLED, 2, true);
OutputPCF8574 outExtLed4(&ioex_extLED, 3, true);

InputButton inputPowerKey(20, false, &outLed1);
//Button powerButton(POWER_KEY);


void setup() {
  Serial.begin(9600);
  Serial.println("startup"); 

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
  
  outLed1.begin();
  outLed2.begin();
  outExtLed1.begin();

  inputPowerKey.begin();
  
  outLed1.write(1);
  outLed2.write(1);
}

void loop()
{
  inputPowerKey.read(); 
}