#ifndef ARDUINO_PCF8574_H
#define ARDUINO_PCF8574_H

#include <Arduino.h>
#include <Wire.h>
class PCF8574 {
    TwoWire* wire = &Wire;
    uint8_t address;
    
    public:
    void attach(TwoWire& w) { wire = &w; }
    void setDeviceAddress(const uint8_t addr) { address = addr; }

    uint8_t input(uint8_t& value)
    {
        return readByte(address, value);
    }

    uint8_t output(const uint8_t bit, const bool value)
    {
        uint8_t ret, dataBuffer = 0;
        /*
        ret = readByte(address, dataBuffer);
        if(ret != 0) {
            return ret;
        }*/
        if(value) {
            dataBuffer |= (0x01 << bit);
        } else {
            dataBuffer &= ~(0x01 << bit);
        }
        return writeByte(address, dataBuffer);
    }

    uint8_t output(const uint8_t value)
    {
        return writeByte(address, value);
    }

    private:
    uint8_t writeByte(const uint8_t address, const uint8_t value) {
        wire->beginTransmission(address);
        wire->write(value);
        return wire->endTransmission();        
    }

    uint8_t readByte(const uint8_t address, uint8_t& value) {
        wire->beginTransmission(address);
        value = wire->read();
        return wire->endTransmission();
    }
};

#endif //ARDUINO_PCF8574_H